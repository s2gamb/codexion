/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:47:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:47:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_all_coders(t_sim *sim)
{
	/*
	** Justification:
	**   Broadcasts on every dongle's condition variable so any coder thread
	**   blocked in `dongle_wait` wakes immediately when the simulation stops
	**   or fails during startup.
	**
	** Callers:
	**   - start_coders (main.c)
	**   - run_sim (main.c)
	**   - check_coder_burnout (monitor.c)
	**   - check_all_completed (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - pthread_cond_broadcast (POSIX)
	**   - pthread_mutex_unlock (POSIX)
	*/
}

static int	check_coder_burnout(t_coder *coder)
{
	/*
	** Justification:
	**   Checks whether a coder's elapsed time since `last_compile_start`
	**   (read under `coder->mutex`) has reached `time_to_burnout`. On burnout,
	**   logs the event, stops the simulation, and wakes all blocked coders.
	**
	** Callers:
	**   - monitor_routine (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - pthread_mutex_unlock (POSIX)
	**   - now_ms (time_utils.c)
	**   - log_burnout (log.c)
	**   - wake_all_coders (monitor.c)
	*/
}

static int	check_all_completed(t_sim *sim)
{
	/*
	** Justification:
	**   Verifies whether every coder has reached `number_of_compiles_required`
	**   by reading each coder's `compile_count` under `coder->mutex`. When all
	**   have completed, sets the stop flag and wakes all coders.
	**
	** Callers:
	**   - monitor_routine (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - pthread_mutex_unlock (POSIX)
	**   - sim_set_stopped (time_utils.c)
	**   - wake_all_coders (monitor.c)
	*/
}

void	*monitor_routine(void *arg)
{
	/*
	** Justification:
	**   Thread entry point for the simulation supervisor. Polls every 500 us
	**   to detect either any coder burnout or completion of all required
	**   compiles, exiting as soon as a termination condition is reached.
	**
	** Callers:
	**   - run_sim (main.c; via pthread_create)
	**
	** Callees:
	**   - sim_is_stopped (time_utils.c)
	**   - check_coder_burnout (monitor.c)
	**   - check_all_completed (monitor.c)
	**   - usleep (POSIX)
	*/
}
