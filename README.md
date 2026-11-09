*This project has been created as part of the 42 curriculum by segambar.*

# Codexion — Concurrency & Resource Scheduling

## Description

**Codexion** is a systems programming project exploring multithreading, concurrency control, and priority-based resource scheduling in C under POSIX threads (`pthread`).

The simulation models $N$ coders sitting around a circular table with $N$ hardware dongles, where each dongle sits between two adjacent coders. To compile their project, a coder must simultaneously acquire both their left and right dongles. The coder cycles through three states:
1. **Compiling**: Holds two dongles for `time_to_compile` milliseconds.
2. **Debugging**: Releases both dongles and debugs for `time_to_debug` milliseconds.
3. **Refactoring**: Refactors code for `time_to_refactor` milliseconds before competing for dongles again.

If a coder fails to begin compiling within `time_to_burnout` milliseconds from the start of their last compilation (or from simulation start), they **burn out**, and the simulation terminates immediately. Alternatively, if every coder successfully completes at least `number_of_compiles_required` compilations, the simulation ends successfully.

The project introduces mandatory dongle cooldowns and two competing arbitration policies:
- **FIFO (First-In, First-Out)**: Coders receive contested dongles in order of request arrival.
- **EDF (Earliest Deadline First)**: Coders with the most urgent deadline (`last_compile_start + time_to_burnout`) receive contested dongles first, preventing premature burnout.

---

## Instructions

### Compilation

Build the executable using the provided Makefile:

```bash
make        # Compiles the 'codexion' binary with -Wall -Wextra -Werror -pthread
make clean  # Removes object files
make fclean # Removes object files and the 'codexion' executable
make re     # Recompiles from scratch
```

### Execution

Run the program with the 8 mandatory parameters:

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

#### Parameter Reference

| Parameter | Type | Description |
|---|---|---|
| `number_of_coders` | Positive integer ($> 0$) | Number of coders and dongles around the table. |
| `time_to_burnout` | Positive integer ($> 0$, ms) | Maximum time allowed between compilation starts before burnout. |
| `time_to_compile` | Non-negative integer ($\ge 0$, ms) | Duration a coder holds 2 dongles to compile. |
| `time_to_debug` | Non-negative integer ($\ge 0$, ms) | Duration spent debugging after releasing dongles. |
| `time_to_refactor` | Non-negative integer ($\ge 0$, ms) | Duration spent refactoring before requesting dongles again. |
| `number_of_compiles_required` | Positive integer ($> 0$) | Simulation stops when all coders compile at least this many times. |
| `dongle_cooldown` | Non-negative integer ($\ge 0$, ms) | Time a dongle remains unusable after being released. |
| `scheduler` | String (`fifo` or `edf`) | Arbitration policy used by dongles to order waiting coders. |

#### Examples

```bash
# 4 coders, no burnout, 2 compiles each under FIFO:
./codexion 4 1500 100 100 100 2 0 fifo

# 5 coders with a 10 ms dongle cooldown under EDF:
./codexion 5 1500 100 100 100 2 10 edf

# Single coder edge case (burnout at 300 ms):
./codexion 1 300 100 100 100 5 0 fifo
```

### Automated Testing

The Makefile includes dedicated test targets:

```bash
make test          # Runs basic sanity suite (invalid args, single coder, FIFO, EDF, burnout)
make test_invalid  # Verifies rejection of bad inputs
make test_single   # Tests 1-coder edge case
make test_fifo     # Tests FIFO scheduling with even and odd coder counts
make test_edf      # Tests EDF scheduling with even and odd coder counts
make test_burnout  # Tests burnout precision and post-burnout log silence
make test_stress   # Runs 50 and 100 concurrent threads
make test_leaks    # Runs Valgrind memory leak check
make test_helgrind # Runs Helgrind thread race check
```

---

## Resources

### Documentation & Literature
- **POSIX Threads (IEEE Std 1003.1c)**: Specifications for `pthread_create`, `pthread_mutex_*`, and `pthread_cond_*`.
- **Dijkstra, Edsger W. (1965)**: *Hierarchical Resource Ordering in Concurrent Programming* (Solution to Dining Philosophers).
- **Coffman, E. G., Elphick, M. J., & Shoshani, A. (1971)**: *System Deadlocks*, Computing Surveys (Coffman's 4 conditions).
- **Liu, C. L., & Layland, J. W. (1973)**: *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (Earliest Deadline First algorithm).
- **Cormen, T. H., et al.**: *Introduction to Algorithms* (Binary Min-Heap and Priority Queues).

### AI Usage Disclosure
In accordance with 42 curriculum guidelines, artificial intelligence was utilized as a pair-programming assistant during development:
- **Architecture & Scaffolding**: Formulating the step-by-step implementation plan and data structure layout conforming to 42 Norm guidelines.
- **Code Optimization & Norm Compliance**: Checking function line limits, variable counts, and ensuring clean separation of modules.
- **Testing Design**: Designing test harnesses for Valgrind and Helgrind to verify zero memory leaks and zero data races.
- **Explanations**: Documenting concurrency invariants, Coffman's conditions, and mutex hierarchies for peer evaluation readiness.

---

## Blocking Cases Handled

### 1. Deadlock Prevention & Coffman's Conditions
For a deadlock to occur, four conditions (Coffman's conditions) must hold simultaneously:
1. **Mutual Exclusion**: Only one thread can hold a dongle at a time.
2. **Hold and Wait**: A coder holds one dongle while waiting for their second dongle.
3. **No Preemption**: A dongle cannot be forcibly confiscated from a compiling coder.
4. **Circular Wait**: Coder $1$ holds Dongle $1$ and waits for Dongle $2$; Coder $2$ holds Dongle $2$ and waits for Dongle $3$; $\dots$; Coder $N$ holds Dongle $N$ and waits for Dongle $1$.

**Solution**: We break the **Circular Wait** condition by enforcing a strict **Resource Hierarchy**:
- Every coder acquires dongles in increasing order of their unique ID: $\min(\text{ID}_L, \text{ID}_R)$ followed by $\max(\text{ID}_L, \text{ID}_R)$.
- Coders $1$ through $N-1$ take their left dongle first, then right.
- Coder $N$ has `dongle_left = N` and `dongle_right = 1`, and therefore takes **Dongle 1 first**, then **Dongle N**.
- Because all threads request resources strictly in ascending resource order, a cycle in the resource allocation graph cannot form. Deadlock is mathematically prevented.

### 2. Starvation Prevention & Scheduling
- Each dongle maintains its own **Min-Heap (Priority Queue)**.
- **FIFO**: Request keys equal the atomic `arrival_counter`. The earliest request has the lowest key and is served first.
- **EDF**: Request keys equal the deadline (`last_compile_start + time_to_burnout`). The coder closest to burning out is prioritized.
- **Deterministic Tie-Breaking**: When two coders have identical deadlines in EDF, ties are broken by `arrival_id` (the coder who arrived first gets priority). No thread is indefinitely bypassed.

### 3. Dongle Cooldown Handling
- When a dongle is released, its release timestamp is updated: `available_at = now_ms() + dongle_cooldown`.
- Waiting threads use `pthread_cond_timedwait` calculated against `available_at`.
- When cooldown elapses, the thread wakes up automatically without requiring a continuous busy-wait loop, conserving CPU resources while ensuring prompt pickup.

### 4. Precise Burnout Detection ($\le 10\text{ ms}$)
- A dedicated background **monitor thread** polls all coders at a $500\ \mu\text{s}$ interval (`usleep(500)`).
- When `now_ms() - last_compile_start >= time_to_burnout`, burnout is logged immediately. The sub-millisecond polling guarantees detection and logging well within the $10\text{ ms}$ threshold required by the subject.

### 5. Log Serialization & Post-Burnout Silence
- Console output is guarded by `log_mutex`.
- In `log_burnout`, `sim_set_stopped(sim)` and the `"burned out"` print occur **atomically under `log_mutex`**.
- Any other thread waiting to print will acquire `log_mutex` only after the burnout message has printed, immediately observe `sim_is_stopped() == 1`, and discard its output. Consequently, **no log is ever printed after burnout**.

### 6. Single Coder Edge Case
- When $N = 1$, only 1 dongle exists (`dongle_right == NULL`).
- The coder acquires Dongle 1, logs `"has taken a dongle"`, and sleeps until the monitor triggers burnout at $t = \text{time\_to\_burnout}$.
- The coder wakes up, releases Dongle 1, and the program exits cleanly with 0 leaks and 0 hangs.

---

## Thread Synchronization Mechanisms

### Synchronization Primitives

| Primitive | Variable | Scope | Purpose |
|---|---|---|---|
| `pthread_mutex_t` | `dongle->mutex` | Per Dongle | Protects dongle state: `is_in_use`, `available_at`, and `heap`. |
| `pthread_cond_t` | `dongle->cond` | Per Dongle | Coordinates waiting coders contending for this dongle. |
| `pthread_mutex_t` | `coder->mutex` | Per Coder | Protects `last_compile_start` and `compile_count` (shared between coder and monitor). |
| `pthread_mutex_t` | `sim->stop_mutex` | Simulation | Protects read and write access to `stop_flag`. |
| `pthread_mutex_t` | `sim->log_mutex` | Simulation | Serializes standard output, preventing interleaved log lines. |
| `pthread_mutex_t` | `sim->counter_mutex`| Simulation | Guarantees atomic increments of `arrival_counter`. |

### Race Condition Prevention Examples

1. **Coder $\leftrightarrow$ Monitor Communication**:
   - The coder writes `last_compile_start = now_ms()` and `compile_count++` inside `coder_compile()`.
   - The monitor reads these fields inside `check_coder_burnout()` and `check_all_completed()`.
   - Both operations are strictly protected by `coder->mutex`. Without this mutex, reading a 64-bit integer while another thread writes to it would produce torn reads and data races (flagged by Helgrind).

2. **Dongle Arbitration & State Machine**:
   - Pushing into the priority queue, checking availability, and popping off the root all happen while holding `dongle->mutex`.
   - When a coder releases a dongle, it updates `available_at`, resets `is_in_use = 0`, and calls `pthread_cond_broadcast(&dongle->cond)` while holding `dongle->mutex`, ensuring waiting threads receive consistent state upon waking.

3. **Safe Shutdown Broadcast**:
   - When the monitor detects burnout or completion, it invokes `wake_all_coders(sim)`.
   - This broadcasts on every `dongle->cond` under its respective mutex, guaranteeing that any coder sleeping in `pthread_cond_timedwait` wakes up, evaluates `sim_is_stopped()`, releases any held dongle, and terminates cleanly.
