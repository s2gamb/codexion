/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:33:32 by segambar          #+#    #+#             */
/*   Updated: 2026/11/05 17:17:03 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return (TRUE);
	return (FALSE);
}

int	check_all_digit(char *arg)
{
	int	l;
	int	i;

	i = 0;
	l = strlen(arg);
	while (i < l)
		if (!is_digit(arg[i++]))
			return (ERROR);
	return (OK);
}

int	parse(int argc, char **argv, t_config *config)
{
	int	n;
	int	error;

	if (argc != 9)
		return (ERROR);
	n = 1;
	while (n < 8)
	{
		error = check_all_digit(argv[n]);
		if (error)
			return (error);
		n++;
	}
	printf("%s\n", argv[8]);
	return (OK);
}
