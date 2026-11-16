/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 20:50:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 20:58:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	now_ms(void)
{
	/*
	** Justification:
	**   Provides a monotonic millisecond timestamp using CLOCK_MONOTONIC so
	**   simulation intervals, burnout checks, and dongle cooldowns are immune
	**   to wall-clock adjustments.
	**
	** Callers:
	**   - elapsed_ms (time_utils.c)
	**   - sleep_ms (time_utils.c)
	**   - run_sim (main.c)
	**   - dongle_wait (dongle.c)
	**   - dongle_acquire (dongle.c)
	**   - dongle_release (dongle.c)
	**   - coder_compile (coder.c)
	**   - check_coder_burnout (monitor.c)
	**
	** Callees:
	**   - clock_gettime (POSIX)
	*/
}

long long	elapsed_ms(long long start)
{
	/*
	** Justification:
	**   Computes milliseconds elapsed since a baseline monotonic timestamp
	**   `start`, used for relative log timestamps and sleep duration checks.
	**
	** Callers:
	**   - sleep_ms (time_utils.c)
	**   - log_msg (log.c)
	**   - log_burnout (log.c)
	**
	** Callees:
	**   - now_ms (time_utils.c)
	*/
}

int	sim_is_stopped(t_sim *sim)
{
	/*
	** Justification:
	**   Thread-safe reader for `sim->stop_flag` protected by `stop_mutex`,
	**   allowing coder, monitor, and logging paths to observe termination
	**   without data races.
	**
	** Callers:
	**   - sleep_ms (time_utils.c)
	**   - log_msg (log.c)
	**   - log_burnout (log.c)
	**   - dongle_acquire (dongle.c)
	**   - coder_single (coder.c)
	**   - coder_debug_and_refactor (coder.c)
	**   - coder_routine (coder.c)
	**   - monitor_routine (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - pthread_mutex_unlock (POSIX)
	*/
}

void	sim_set_stopped(t_sim *sim)
{
	/*
	** Justification:
	**   Thread-safe writer that sets `sim->stop_flag = 1` under `stop_mutex`
	**   to signal all threads to exit cleanly upon burnout, completion, or
	**   startup failure.
	**
	** Callers:
	**   - start_coders (main.c)
	**   - run_sim (main.c)
	**   - log_burnout (log.c)
	**   - check_all_completed (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - pthread_mutex_unlock (POSIX)
	*/
}

void	sleep_ms(t_sim *sim, long long ms)
{
	/*
	** Justification:
	**   Interruptible millisecond sleep that polls `sim_is_stopped` in 500 us
	**   increments until `ms` milliseconds have elapsed, ensuring coder threads
	**   wake promptly when the simulation stops instead of blocking for long
	**   compile/debug/refactor durations.
	**
	** Callers:
	**   - coder_compile (coder.c)
	**   - coder_debug_and_refactor (coder.c)
	**
	** Callees:
	**   - now_ms (time_utils.c)
	**   - sim_is_stopped (time_utils.c)
	**   - elapsed_ms (time_utils.c)
	**   - usleep (POSIX)
	*/
}
