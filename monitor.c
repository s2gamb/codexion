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
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

static int	check_coder_burnout(t_coder *coder)
{
	long long	last_compile;

	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->mutex);
	if (now_ms() - last_compile >= coder->sim->config.time_to_burnout)
	{
		log_burnout(coder);
		wake_all_coders(coder->sim);
		return (TRUE);
	}
	return (FALSE);
}

static int	check_all_completed(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].mutex);
		if (count < sim->config.number_of_compiles_required)
			return (FALSE);
		i++;
	}
	sim_set_stopped(sim);
	wake_all_coders(sim);
	return (TRUE);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			if (check_coder_burnout(&sim->coders[i]))
				return (NULL);
			i++;
		}
		if (check_all_completed(sim))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}
