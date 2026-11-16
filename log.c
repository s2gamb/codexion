/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 20:58:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 20:58:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_msg(t_coder *coder, const char *msg)
{
	/*
	** Justification:
	**   Serializes state-transition stdout logging under `log_mutex` so output
	**   lines never interleave across threads, and checks `sim_is_stopped`
	**   while holding `log_mutex` so no normal status is printed after burnout
	**   or simulation completion.
	**
	** Callers:
	**   - dongle_acquire (dongle.c)
	**   - coder_compile (coder.c)
	**   - coder_debug_and_refactor (coder.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - sim_is_stopped (time_utils.c)
	**   - elapsed_ms (time_utils.c)
	**   - printf (libc)
	**   - pthread_mutex_unlock (POSIX)
	*/
}

void	log_burnout(t_coder *coder)
{
	/*
	** Justification:
	**   Atomically checks and sets the simulation stop flag under `log_mutex`
	**   and prints the burnout message exactly once for the first coder that
	**   exceeds `time_to_burnout`, preventing post-burnout messages from racing.
	**
	** Callers:
	**   - check_coder_burnout (monitor.c)
	**
	** Callees:
	**   - pthread_mutex_lock (POSIX)
	**   - sim_is_stopped (time_utils.c)
	**   - sim_set_stopped (time_utils.c)
	**   - elapsed_ms (time_utils.c)
	**   - printf (libc)
	**   - pthread_mutex_unlock (POSIX)
	*/
}
