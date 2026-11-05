/* ************************************************************************** */
/*										  */
/*							  :::	   ::::::::   */
/*	 codexion.h											:+:		 :+:	:+:   */
/*							  +:+ +:+		  +:+	  */
/*	 By: segambar <serge_gamb@yahoo.com>		+#+  +:+	   +#+		  */
/*						  +#+#+#+#+#+	+#+		  */
/*	 Created: 2026/11/03 14:36:36 by segambar		   #+#	  #+#		  */
/*	 Updated: 2026/11/03 14:38:50 by segambar		  ###	########.fr		  */
/*										  */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <string.h>

# define ERROR 1
# define TRUE 1
# define FALSE 0
# define OK 0

typedef struct s_config
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles_required;
	int	dongle_cooldown;
	int	scheduler;
}	t_config;

int	parse(int argc, char **argv, t_config *config);

#endif
