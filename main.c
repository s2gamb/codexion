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
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static int	start_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			sim_set_stopped(sim);
			wake_all_coders(sim);
			join_coders(sim, i);
			return (ERROR);
		}
		i++;
	}
	return (OK);
}

static int	run_sim(t_sim *sim)
{
	sim->start_time = now_ms();
	if (start_coders(sim) != OK)
		return (ERROR);
	if (pthread_create(&sim->monitor_thread, NULL,
			monitor_routine, sim) != 0)
	{
		sim_set_stopped(sim);
		wake_all_coders(sim);
		join_coders(sim, sim->config.number_of_coders);
		return (ERROR);
	}
	pthread_join(sim->monitor_thread, NULL);
	join_coders(sim, sim->config.number_of_coders);
	return (OK);
}

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;

	if (parse(argc, argv, &config) != OK)
		return (ERROR);
	if (init_sim(&sim, &config) != OK)
		return (ERROR);
	run_sim(&sim);
	cleanup_sim(&sim);
	return (OK);
}
