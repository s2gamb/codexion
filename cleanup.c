/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:25:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:25:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_coders(t_sim *sim, int count)
{
	/*
	** Justification:
	**   Destroys the per-coder mutex for the first `count` coders and frees
	**   the `sim->coders` array. Taking `count` supports both full teardown
	**   and partial rollback during init_coders failure.
	**
	** Callers:
	**   - init_coders (init.c)
	**   - cleanup_sim (cleanup.c)
	**
	** Callees:
	**   - pthread_mutex_destroy (POSIX)
	**   - free (libc)
	*/
}

void	cleanup_dongles(t_sim *sim, int count)
{
	/*
	** Justification:
	**   Destroys the mutex, condition variable, and request priority heap for
	**   the first `count` dongles and frees `sim->dongles`. Taking `count`
	**   supports both full teardown and partial rollback during init_dongles.
	**
	** Callers:
	**   - init_dongles (init.c)
	**   - cleanup_sim (cleanup.c)
	**
	** Callees:
	**   - pthread_mutex_destroy (POSIX)
	**   - pthread_cond_destroy (POSIX)
	**   - heap_destroy (heap_utils.c)
	**   - free (libc)
	*/
}

void	cleanup_sim(t_sim *sim)
{
	/*
	** Justification:
	**   Releases all simulation resources: tears down coders and dongles, and
	**   destroys the simulation-wide `stop_mutex`, `log_mutex`, and
	**   `counter_mutex`. Safe to call on partially initialized `t_sim`.
	**
	** Callers:
	**   - init_sim (init.c)
	**   - main (main.c)
	**
	** Callees:
	**   - cleanup_coders (cleanup.c)
	**   - cleanup_dongles (cleanup.c)
	**   - pthread_mutex_destroy (POSIX)
	*/
}
