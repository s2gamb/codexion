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
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (ERROR);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (ERROR);
	}
	if (pthread_mutex_init(&sim->counter_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->log_mutex);
		return (ERROR);
	}
	return (OK);
}

static int	init_dongle_item(t_sim *sim, int i, int n)
{
	sim->dongles[i].id = i + 1;
	sim->dongles[i].available_at = 0;
	sim->dongles[i].is_in_use = 0;
	if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
		return (ERROR);
	if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		return (ERROR);
	}
	if (heap_init(&sim->dongles[i].heap, n + 2) != OK)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		return (ERROR);
	}
	return (OK);
}

static int	init_dongles(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	sim->dongles = (t_dongle *)malloc(sizeof(t_dongle) * n);
	if (!sim->dongles)
		return (ERROR);
	i = 0;
	while (i < n)
	{
		if (init_dongle_item(sim, i, n) != OK)
		{
			cleanup_dongles(sim, i);
			return (ERROR);
		}
		i++;
	}
	return (OK);
}

static int	init_coders(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	sim->coders = (t_coder *)malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (ERROR);
	memset(sim->coders, 0, sizeof(t_coder) * n);
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].dongle_left = &sim->dongles[i];
		if (n > 1)
			sim->coders[i].dongle_right = &sim->dongles[(i + 1) % n];
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
		{
			cleanup_coders(sim, i);
			return (ERROR);
		}
		i++;
	}
	return (OK);
}

int	init_sim(t_sim *sim, t_config *config)
{
	sim->config = *config;
	sim->start_time = 0;
	sim->stop_flag = 0;
	sim->arrival_counter = 0;
	sim->coders = NULL;
	sim->dongles = NULL;
	if (init_sim_mutexes(sim) != OK)
		return (ERROR);
	if (init_dongles(sim) != OK)
	{
		cleanup_sim(sim);
		return (ERROR);
	}
	if (init_coders(sim) != OK)
	{
		cleanup_sim(sim);
		return (ERROR);
	}
	return (OK);
}
