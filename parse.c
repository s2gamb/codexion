/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:33:32 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 20:08:48 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_scheduler(const char *str, int *scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = SCHEDULER_FIFO;
		return (OK);
	}
	if (strcmp(str, "edf") == 0)
	{
		*scheduler = SCHEDULER_EDF;
		return (OK);
	}
	return (ERROR);
}

static int	parse_int(const char *str, int *out, long min)
{
	long	num;
	int		i;

	if (!str || str[0] == '\0')
		return (ERROR);
	num = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (ERROR);
		num = num * 10 + (str[i] - '0');
		if (num > INT_MAX)
			return (ERROR);
		i++;
	}
	if (num < min)
		return (ERROR);
	*out = (int)num;
	return (OK);
}

static int	populate_config(t_config *config, char **argv)
{
	int	error;

	error = parse_int(argv[1], &config->number_of_coders, 1);
	if (error)
		return (error);
	error = parse_int(argv[2], &config->time_to_burnout, 1);
	if (error)
		return (error);
	error = parse_int(argv[3], &config->time_to_compile, 0);
	if (error)
		return (error);
	error = parse_int(argv[4], &config->time_to_debug, 0);
	if (error)
		return (error);
	error = parse_int(argv[5], &config->time_to_refactor, 0);
	if (error)
		return (error);
	error = parse_int(argv[6], &config->number_of_compiles_required, 1);
	if (error)
		return (error);
	error = parse_int(argv[7], &config->dongle_cooldown, 0);
	if (error)
		return (error);
	return (OK);
}

int	parse(int argc, char **argv, t_config *config)
{
	int	error;

	if (argc != 9)
		return (ERROR);
	error = populate_config(config, argv);
	if (error)
		return (error);
	error = parse_scheduler(argv[8], &config->scheduler);
	if (error)
		return (error);
	return (OK);
}
