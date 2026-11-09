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
	pthread_mutex_lock(&coder->sim->log_mutex);
	if (!sim_is_stopped(coder->sim))
	{
		printf("%lld %d %s\n", elapsed_ms(coder->sim->start_time),
			coder->id, msg);
	}
	pthread_mutex_unlock(&coder->sim->log_mutex);
}

void	log_burnout(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->log_mutex);
	if (!sim_is_stopped(coder->sim))
	{
		sim_set_stopped(coder->sim);
		printf("%lld %d burned out\n", elapsed_ms(coder->sim->start_time),
			coder->id);
	}
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
