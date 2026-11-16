/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:25:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:25:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_sim_mutexes(t_sim *sim)
{
	/*
	** Justification:
	**   Initializes the simulation-wide synchronization mutexes (`stop_mutex`,
	**   `log_mutex`, and `counter_mutex`) with rollback destruction of any
	**   previously initialized mutex if a later initialization fails.
	**
	** Callers:
	**   - init_sim (init.c)
	**
	** Callees:
	**   - pthread_mutex_init (POSIX)
	**   - pthread_mutex_destroy (POSIX)
	*/
}

static int	init_dongle_item(t_sim *sim, int i, int n)
{
	/*
	** Justification:
	**   Initializes a single dongle at index `i`: assigns its 1-based ID,
	**   resets availability/usage state, initializes its mutex and condition
	**   variable, and allocates its priority-queue heap with capacity `n + 2`.
	**   Rolls back mutex/cond resources on partial failure.
	**
	** Callers:
	**   - init_dongles (init.c)
	**
	** Callees:
	**   - pthread_mutex_init (POSIX)
	**   - pthread_cond_init (POSIX)
	**   - pthread_mutex_destroy (POSIX)
	**   - pthread_cond_destroy (POSIX)
	**   - heap_init (heap_utils.c)
	*/
}

static int	init_dongles(t_sim *sim)
{
	/*
	** Justification:
	**   Allocates the `t_dongle` array for `number_of_coders` dongles and
	**   initializes each entry sequentially. If any item fails to initialize,
	**   cleans up all previously initialized dongles to prevent leaks.
	**
	** Callers:
	**   - init_sim (init.c)
	**
	** Callees:
	**   - malloc (libc)
	**   - init_dongle_item (init.c)
	**   - cleanup_dongles (cleanup.c)
	*/
}

static int	init_coders(t_sim *sim)
{
	/*
	** Justification:
	**   Allocates and zero-initializes the `t_coder` array, assigns 1-based
	**   coder IDs, wires left/right dongle pointers in a circular topology
	**   (leaving `dongle_right` NULL when `number_of_coders == 1`), and
	**   initializes each coder's mutex with rollback on failure.
	**
	** Callers:
	**   - init_sim (init.c)
	**
	** Callees:
	**   - malloc (libc)
	**   - memset (libc)
	**   - pthread_mutex_init (POSIX)
	**   - cleanup_coders (cleanup.c)
	*/
}

int	init_sim(t_sim *sim, t_config *config)
{
	/*
	** Justification:
	**   Initializes the entire `t_sim` structure from a validated `t_config`:
	**   resets counters/flags/pointers, initializes global mutexes, allocates
	**   and initializes dongles, and allocates and initializes coders, calling
	**   cleanup_sim on failure so no partially initialized state leaks.
	**
	** Callers:
	**   - main (main.c)
	**
	** Callees:
	**   - init_sim_mutexes (init.c)
	**   - init_dongles (init.c)
	**   - init_coders (init.c)
	**   - cleanup_sim (cleanup.c)
	*/
}
