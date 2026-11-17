/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:41:06 by segambar          #+#    #+#             */
/*   Updated: 2026/11/17 17:52:18 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
static void	join_coders(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

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
static int	start_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]))
		{
			sim_set_stopped(sim);
			wake_all_coders(sim);
			join_coders(sim, i);
			return (ERROR);
		}
		i++;
	}
	return (SUCCESS);
}

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
static int	run_sim(t_sim *sim)
{
	sim->start_time = now_ms();
	if (start_coders(sim))
		return (ERROR);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
	{
		sim_set_stopped(sim);
		wake_all_coders(sim);
		join_coders(sim, sim->config.number_of_coders);
		return (ERROR);
	}
	join_coders(sim, sim->config.number_of_coders);
	pthread_join(sim->monitor_thread, NULL);
	return (SUCCESS);
}

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
int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;
	int			error;

	error = parse(argc, argv, &config);
	if (error)
		return (1);
	init_sim(&sim, &config);
	run_sim(&sim);
	cleanup_sim(&sim);
}
