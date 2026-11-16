/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:41:06 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:47:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_coders(t_sim *sim, int count)
{
	/*
	** Justification:
	**   Waits for the first `count` coder threads to terminate via pthread_join.
	**   Accepting an explicit `count` allows partial rollback when thread
	**   creation fails mid-loop in start_coders, as well as full shutdown
	**   joining in run_sim after the monitor finishes.
	**
	** Callers:
	**   - start_coders (main.c)
	**   - run_sim (main.c)
	**
	** Callees:
	**   - pthread_join (POSIX)
	*/
}

static int	start_coders(t_sim *sim)
{
	/*
	** Justification:
	**   Initializes each coder's `last_compile_start` to `sim->start_time`
	**   and spawns `number_of_coders` worker threads running `coder_routine`.
	**   If any pthread_create call fails, it stops the simulation, wakes any
	**   already-blocked coder threads, and joins only the threads created so
	**   far before returning ERROR.
	**
	** Callers:
	**   - run_sim (main.c)
	**
	** Callees:
	**   - pthread_create (POSIX; entry point: coder_routine)
	**   - sim_set_stopped (time_utils.c)
	**   - wake_all_coders (monitor.c)
	**   - join_coders (main.c)
	*/
}

static int	run_sim(t_sim *sim)
{
	/*
	** Justification:
	**   Orchestrates the active simulation lifecycle: records `sim->start_time`,
	**   launches all coder threads, and starts the dedicated monitor thread.
	**   If monitor creation fails, it signals stop, wakes all coders, and joins
	**   them; otherwise it joins the monitor thread first and then all coders.
	**
	** Callers:
	**   - main (main.c)
	**
	** Callees:
	**   - now_ms (time_utils.c)
	**   - start_coders (main.c)
	**   - pthread_create (POSIX; entry point: monitor_routine)
	**   - sim_set_stopped (time_utils.c)
	**   - wake_all_coders (monitor.c)
	**   - join_coders (main.c)
	**   - pthread_join (POSIX)
	*/
}

int	main(int argc, char **argv)
{
	/*
	** Justification:
	**   Program entry point. Parses and validates CLI arguments into `t_config`,
	**   initializes the simulation state (`t_sim`), executes the concurrent
	**   simulation, and guarantees resource cleanup before exiting.
	**
	** Callers:
	**   - C runtime / OS process entry point
	**
	** Callees:
	**   - parse (parse.c)
	**   - init_sim (init.c)
	**   - run_sim (main.c)
	**   - cleanup_sim (cleanup.c)
	*/
}
