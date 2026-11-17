/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:33:32 by segambar          #+#    #+#             */
/*   Updated: 2026/11/17 17:23:29 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Justification:
**   Translates the scheduler CLI argument string ("fifo" or "edf") into
**   the internal SCHEDULER_FIFO or SCHEDULER_EDF constant, rejecting any
**   unrecognized scheduling policy with ERROR.
**
** Callers:
**   - parse (parse.c)
**
** Callees:
**   - strcmp (libc)
*/
static int	parse_scheduler(const char *str, int *scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = SCHEDULER_FIFO;
		return (SUCCESS);
	}
	else if (strcmp(str, "edf") == 0)
	{
		*scheduler = SCHEDULER_EDF;
		return (SUCCESS);
	}
	return (ERROR);
}

/*
** Justification:
**   Strictly parses a non-empty, digit-only string into a bounded `int`
**   value (>= `min` and <= INT_MAX), avoiding undefined overflow or
**   trailing-character acceptance found in atoi/strtol wrappers.
**
** Callers:
**   - populate_config (parse.c)
**
** Callees:
**   - None
*/
static int	parse_int(const char *str, int *out, long min)
{
	long	value;

	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (ERROR);
		value = value * 10 + (*str - '0');
		if (value > INT_MAX)
			return (ERROR);
		str++;
	}
	if (value < min)
		return (ERROR);
	*out = (int)value;
	return (SUCCESS);
}

/*
** Justification:
**   Populates all seven numeric simulation parameters in `t_config` from
**   argv[1..7], enforcing parameter-specific lower bounds (minimum 1 for
**   coder count, burnout time, and required compiles; minimum 0 for
**   compile/debug/refactor durations and dongle cooldown).
**
** Callers:
**   - parse (parse.c)
**
** Callees:
**   - parse_int (parse.c)
*/
static int	populate_config(t_config *config, char **argv)
{
	if (parse_int(argv[1], &config->number_of_coders, 1))
		return (ERROR);
	if (parse_int(argv[2], &config->burnout_time, 1))
		return (ERROR);
	if (parse_int(argv[3], &config->required_compiles, 1))
		return (ERROR);
	if (parse_int(argv[4], &config->compile_duration, 0))
		return (ERROR);
	if (parse_int(argv[5], &config->debug_duration, 0))
		return (ERROR);
	if (parse_int(argv[6], &config->refactor_duration, 0))
		return (ERROR);
	if (parse_int(argv[7], &config->dongle_cooldown, 0))
		return (ERROR);
	if (parse_scheduler(argv[8], &config->scheduler))
		return (ERROR);
	return (SUCCESS);
}

/*
** Justification:
**   Top-level CLI argument validator. Verifies the exact argument count
**   (argc == 9), delegates numeric configuration parsing to populate_config,
**   and parses the scheduler policy from argv[8].
**
** Callers:
**   - main (main.c)
**
** Callees:
**   - populate_config (parse.c)
**   - parse_scheduler (parse.c)
*/
int	parse(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		return (ERROR);
	return (populate_config(config, argv));
}
