/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:35:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:35:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority_key(t_coder *coder, long long arrival)
{
	long long	key;

	if (coder->sim->config.scheduler == SCHEDULER_FIFO)
		return (arrival);
	pthread_mutex_lock(&coder->mutex);
	key = coder->last_compile_start + coder->sim->config.time_to_burnout;
	pthread_mutex_unlock(&coder->mutex);
	return (key);
}

static void	dongle_wait(t_dongle *dongle)
{
	struct timespec	ts;
	long long		now;
	long long		wait_ms;

	now = now_ms();
	wait_ms = 5;
	if (dongle->available_at > now)
	{
		wait_ms = dongle->available_at - now;
		if (wait_ms > 10)
			wait_ms = 10;
	}
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += wait_ms * 1000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec %= 1000000000;
	}
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

static int	register_request(t_dongle *dongle, t_coder *coder)
{
	t_request	req;

	req.coder_id = coder->id;
	pthread_mutex_lock(&coder->sim->counter_mutex);
	coder->sim->arrival_counter++;
	req.arrival_id = coder->sim->arrival_counter;
	pthread_mutex_unlock(&coder->sim->counter_mutex);
	req.priority_key = get_priority_key(coder, req.arrival_id);
	pthread_mutex_lock(&dongle->mutex);
	if (heap_push(&dongle->heap, req) != OK)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (ERROR);
	}
	return (OK);
}

int	dongle_acquire(t_dongle *dongle, t_coder *coder)
{
	t_request	top;

	if (register_request(dongle, coder) != OK)
		return (ERROR);
	while (!sim_is_stopped(coder->sim))
	{
		if (!dongle->is_in_use && now_ms() >= dongle->available_at)
		{
			if (heap_peek(&dongle->heap, &top) == OK
				&& top.coder_id == coder->id)
				break ;
		}
		dongle_wait(dongle);
	}
	if (sim_is_stopped(coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (ERROR);
	}
	heap_pop(&dongle->heap, NULL);
	dongle->is_in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	log_msg(coder, MSG_TAKEN);
	return (OK);
}

void	dongle_release(t_dongle *dongle, t_coder *coder)
{
	if (!dongle)
		return ;
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_in_use = 0;
	dongle->available_at = now_ms() + coder->sim->config.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
