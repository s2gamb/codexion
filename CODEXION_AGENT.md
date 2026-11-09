# Codexion: Architecture and Agent Instructions

This document describes the planned architecture for the Codexion project (42 curriculum, C, POSIX threads). Read it fully before writing or suggesting any code.

## 1. Role of the agent

The project owner is a learner who must be able to explain every line during peer evaluation. Therefore:

- Work **one step at a time**, following the build order in section 9. Do not generate the whole project at once.
- After each step, **explain** what each function does and why each mutex/condition variable exists.
- Prefer small, readable functions. Never add code the learner has not understood.
- Ask before making architectural changes that deviate from this document.
- Suggest tests after each step.

## 2. Project summary

N coders sit in a circle with N dongles (one between each pair of neighbors). A coder needs **two** dongles to compile, then debugs, then refactors, then repeats. If a coder does not start a compile within `time_to_burnout` ms of the start of their last compile (or of the simulation), they burn out and the simulation stops. The simulation also stops when every coder has compiled at least `number_of_compiles_required` times.

Program arguments (all mandatory, all validated):

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

- All times are in milliseconds. Reject negative numbers, non-integers, and any scheduler other than `fifo` or `edf`.
- `dongle_cooldown`: after release, a dongle cannot be taken again until the cooldown has elapsed.
- `scheduler`: decides who gets a dongle when several coders wait for it.
  - `fifo`: the earliest request is served first.
  - `edf`: the coder with the earliest deadline is served first, where `deadline = last_compile_start + time_to_burnout`. Since `time_to_burnout` is the same for all coders, this means the coder who started compiling longest ago goes first. A deterministic tie-breaker is required.

Required log format (one line per state change, never interleaved):

```
<timestamp_ms> <id> has taken a dongle
<timestamp_ms> <id> is compiling
<timestamp_ms> <id> is debugging
<timestamp_ms> <id> is refactoring
<timestamp_ms> <id> burned out
```

The burnout message must be printed within 10 ms of the actual burnout.

## 3. Hard constraints (the subject)

- Language: C. Compile with `cc -Wall -Wextra -Werror -pthread`.
- **No global variables.**
- Allowed external functions: `pthread_create`, `pthread_join`, `pthread_mutex_init/lock/unlock/destroy`, `pthread_cond_init/wait/timedwait/signal/broadcast/destroy`, `gettimeofday`, `clock_gettime`, `usleep`, `write`, `malloc`, `free`, `printf`, `fprintf`, `strcmp`, `strlen`, `atoi`, `memset`. Libft is **not** allowed.
- Each coder is a thread. A **separate monitor thread** detects burnout and completion.
- Each dongle's state is protected by a mutex. Condition variables may be used for waiting.
- Logging is serialized with a mutex.
- A **priority queue (heap) must be implemented by hand** for fifo/edf scheduling.
- With one coder there is only one dongle (that coder can never compile and must burn out cleanly, without hanging).
- No memory leaks, no crashes, no data races, no deadlocks.
- Makefile at the root with rules `NAME`, `all`, `clean`, `fclean`, `re`, no unnecessary relinking.
- A `README.md` is required (see section 11).

## 4. 42 Norm reminders

Verify against the official Norm document, but plan for:

- Maximum 25 lines per function.
- Maximum 5 functions per file.
- Maximum 4 parameters per function.
- Maximum 5 variable declarations per function.
- Declarations at the top of the function, no `for` loops or ternaries (as defined by the current Norm version).

Because of the parameter limit, **pass structs by pointer** instead of many arguments. A norm error results in a zero, so keep functions small from the start. You can alwayes use `norminette ./*.c` or `norminette codexion.h` to check against norm errors.

## 5. Data structures (`codexion.h`)

| Struct | Contents |
|---|---|
| `t_config` | Parsed arguments: coder count, burnout/compile/debug/refactor times, required compiles, dongle cooldown, scheduler type. |
| `t_request` | A heap entry: coder id, priority key, arrival counter (tie-breaker). |
| `t_heap` | Array of `t_request`, size, capacity. |
| `t_dongle` | Mutex, condition variable, `available_at` timestamp (cooldown), in-use flag, heap of waiting coders. |
| `t_coder` | Id, thread handle, pointers to left and right dongle, `last_compile_start`, `compile_count`, its own mutex, pointer to `t_sim`. |
| `t_sim` | `t_config`, start time, stop flag and its mutex, log mutex, arrays of coders and dongles, arrival counter. |

## 6. File layout

| File | Purpose |
|---|---|
| `main.c` | Parse, init, run, cleanup. Nothing else. |
| `parse.c` | Validate and convert arguments, reject bad input. |
| `init.c` | Allocate and initialize `t_sim`, dongles, coders, mutexes. |
| `cleanup.c` | Destroy mutexes and condition variables, free all memory. |
| `time_utils.c` | `now_ms`, elapsed time since simulation start, interruptible sleep. |
| `log.c` | The only place that prints. Always under the log mutex. |
| `heap.c` | Push, pop, sift up, sift down. |
| `heap_utils.c` | Heap init, destroy, compare. |
| `dongle.c` | Acquire and release a dongle, cooldown wait, scheduling. |
| `coder.c` | Thread routine and compile/debug/refactor cycle. |
| `monitor.c` | Burnout and completion watcher. |

If a file exceeds 5 functions, split it (for example `dongle.c` into `dongle_acquire.c` and `dongle_release.c`).

## 7. Design decisions

### 7.1 One heap, one key

The scheduler is selected by a single small function that computes the **priority key** of a request:

- `fifo`: key = arrival counter.
- `edf`: key = `last_compile_start + time_to_burnout`; ties are broken by the arrival counter.

The heap code is identical for both schedulers. Only the key calculation differs.

### 7.2 Dongle acquisition

1. Lock the dongle's mutex.
2. Push a request into the dongle's heap.
3. Wait on the condition variable until all of these are true: the dongle is not in use, the cooldown has passed (`now >= available_at`), and this coder is at the top of the heap. Use `pthread_cond_timedwait` so the cooldown expiry and the stop flag can be handled.
4. Pop the request, mark the dongle in use, unlock.

Release: lock, mark free, set `available_at = now + dongle_cooldown`, broadcast, unlock.

### 7.3 Coder thread

Loop: acquire both dongles, log, compile (set `last_compile_start`, increment `compile_count` under the coder's mutex), release both dongles, debug, refactor. Check the stop flag between every step and during waits.

### 7.4 Monitor thread

Loop: for each coder, read `last_compile_start` (under the coder's mutex) and compare with now. If `now - last_compile_start >= time_to_burnout`, set the stop flag, log `burned out` exactly once, and wake all waiting coders (broadcast on every dongle) so they can exit. Also check whether every coder reached `number_of_compiles_required`. Poll at a short interval (1 ms or less) to respect the 10 ms precision requirement.

### 7.5 Shared data protection

- `last_compile_start` and `compile_count` are written by the coder and read by the monitor, so each coder has a small mutex.
- The stop flag has its own mutex, accessed through helper functions.
- Only `log.c` prints, and it checks the stop flag so nothing is printed after the burnout message.

## 8. Open decisions (resolve with the learner, do not decide silently)

- **Deadlock prevention**: for example, odd and even coders taking dongles in different orders, and how this interacts with the heap-based waiting. Document it in terms of Coffman's conditions.
- **Single-coder case**: a clean path with one dongle, one `has taken a dongle` log, then burnout.
- **Interruptible sleep**: sleep in small steps, checking the stop flag, so threads exit quickly.
- **Starvation under EDF**: confirm liveness for feasible parameters.
- **Time source**: `gettimeofday` or `clock_gettime`, with a single shared start time.

## 9. Build order

1. `codexion.h` (structs), `parse.c`, `time_utils.c`, `log.c`
2. `heap.c`, `heap_utils.c` (test with a small standalone test program)
3. `init.c`, `cleanup.c`
4. One coder, then several coders with `fifo`
5. `monitor.c`
6. `edf`
7. Edge cases (single coder, tight timings, large coder counts)
8. Norm check, leak check, race check, README

Test after every step before moving on.

## 10. Testing checklist

- Invalid arguments are rejected (negative, non-integer, wrong scheduler, wrong argument count).
- No data races: run with `valgrind --tool=helgrind` and `--tool=drd`, and/or compile with `-fsanitize=thread`.
- No leaks: `valgrind --leak-check=full`.
- Burnout log appears within 10 ms of the real burnout.
- Simulation stops cleanly when all coders reach the required compile count.
- Logs never interleave and nothing prints after `burned out`.
- Cases to try: 1 coder, 2 coders, odd and even counts, 200 coders, very small and very large times, `fifo` and `edf`.

## 11. README requirements

`README.md` at the repository root, in English, with:

- First line italicized: `*This project has been created as part of the 42 curriculum by <login1>[, <login2>...].*`
- **Description**: goal and overview.
- **Instructions**: compilation and execution.
- **Resources**: references (docs, articles, tutorials) and a description of how AI was used, for which tasks and parts.
- **Blocking cases handled**: deadlock prevention and Coffman's conditions, starvation prevention, cooldown handling, precise burnout detection, log serialization.
- **Thread synchronization mechanisms**: the mutexes and condition variables used, how they protect dongles, logging, and monitor state, with examples of how race conditions are prevented and how coders and the monitor communicate safely.

## 12. Evaluation reminder

During peer evaluation the learner may be asked to make a small code change on the spot. Every part of the code must be understood by the learner, so keep it simple, well-named, and explained.
