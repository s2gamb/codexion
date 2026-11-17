/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:25:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/17 17:28:38 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
void	cleanup_coders(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->coders[i].mutex);
		i++;
	}
	free(sim->coders);
}

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
void	cleanup_dongles(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_destroy(&sim->dongles[i].request_heap);
		i++;
	}
	free(sim->dongles);
}

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
void	cleanup_sim(t_sim *sim)
{
	cleanup_coders(sim, 0);
	cleanup_dongles(sim, 0);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->counter_mutex);
}
