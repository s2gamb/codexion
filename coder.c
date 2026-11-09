/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:35:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:35:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_single(t_coder *coder)
{
	dongle_acquire(coder->dongle_left, coder);
	while (!sim_is_stopped(coder->sim))
		usleep(500);
	dongle_release(coder->dongle_left, coder);
}

static int	coder_take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->dongle_left;
	second = coder->dongle_right;
	if (first->id > second->id)
	{
		first = coder->dongle_right;
		second = coder->dongle_left;
	}
	if (dongle_acquire(first, coder) != OK)
		return (ERROR);
	if (dongle_acquire(second, coder) != OK)
	{
		dongle_release(first, coder);
		return (ERROR);
	}
	return (OK);
}

static void	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = now_ms();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
	log_msg(coder, MSG_COMPILE);
	sleep_ms(coder->sim, coder->sim->config.time_to_compile);
}

static void	coder_debug_and_refactor(t_coder *coder)
{
	log_msg(coder, MSG_DEBUG);
	sleep_ms(coder->sim, coder->sim->config.time_to_debug);
	if (sim_is_stopped(coder->sim))
		return ;
	log_msg(coder, MSG_REFACTOR);
	sleep_ms(coder->sim, coder->sim->config.time_to_refactor);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (!coder->dongle_right)
	{
		coder_single(coder);
		return (NULL);
	}
	while (!sim_is_stopped(coder->sim))
	{
		if (coder_take_dongles(coder) != OK)
			break ;
		coder_compile(coder);
		dongle_release(coder->dongle_left, coder);
		dongle_release(coder->dongle_right, coder);
		if (sim_is_stopped(coder->sim))
			break ;
		coder_debug_and_refactor(coder);
	}
	return (NULL);
}
