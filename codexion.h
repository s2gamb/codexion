/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:36:36 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 20:54:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <limits.h>
# include <time.h>
# include <unistd.h>
# include <pthread.h>

# define ERROR 1
# define TRUE 1
# define FALSE 0
# define OK 0
# define SCHEDULER_FIFO 1
# define SCHEDULER_EDF 2

# define MSG_TAKEN "has taken a dongle"
# define MSG_COMPILE "is compiling"
# define MSG_DEBUG "is debugging"
# define MSG_REFACTOR "is refactoring"

typedef struct s_sim	t_sim;

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

typedef struct s_request
{
	int			coder_id;
	long long	priority_key;
	long long	arrival_id;
}	t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		available_at;
	int				is_in_use;
	t_heap			heap;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*dongle_left;
	t_dongle		*dongle_right;
	long long		last_compile_start;
	int				compile_count;
	pthread_mutex_t	mutex;
	t_sim			*sim;
}	t_coder;

struct s_sim
{
	t_config		config;
	long long		start_time;
	int				stop_flag;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	counter_mutex;
	long long		arrival_counter;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;
};

int			parse(int argc, char **argv, t_config *config);

long long	now_ms(void);
long long	elapsed_ms(long long start);
int			sim_is_stopped(t_sim *sim);
void		sim_set_stopped(t_sim *sim);
void		sleep_ms(t_sim *sim, long long ms);

void		log_msg(t_coder *coder, const char *msg);
void		log_burnout(t_coder *coder);

int			heap_compare(t_request *a, t_request *b);
int			heap_init(t_heap *heap, int capacity);
void		heap_destroy(t_heap *heap);
int			heap_peek(t_heap *heap, t_request *out);
int			heap_push(t_heap *heap, t_request req);
int			heap_pop(t_heap *heap, t_request *out);

int			init_sim(t_sim *sim, t_config *config);
void		cleanup_coders(t_sim *sim, int count);
void		cleanup_dongles(t_sim *sim, int count);
void		cleanup_sim(t_sim *sim);

#endif
