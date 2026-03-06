/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 14:23:12 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/05 17:35:45 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_coder	t_coder;

typedef enum e_scheduler
{
	CODEXION_FIFO,
	CODEXION_EDF
}	t_scheduler;

typedef struct s_dongle
{
	pthread_mutex_t	m;
	pthread_cond_t	cv;
	long			available_at_ms;
}	t_dongle;

typedef struct s_rules
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown_ms;
	t_scheduler	scheduler;
}	t_rules;

typedef struct s_sim
{
	t_rules			rules;

	int				coder_count;
	int				dongle_count;
	t_coder			*coders;
	pthread_t		*threads;
	long			start_ms;
	pthread_mutex_t	log_mutex;
	t_dongle		*dongles;

	int				stop;
	pthread_mutex_t	stop_mutex;

	pthread_mutex_t sched_mutex;
	pthread_cond_t 	sched_cv;
	long 			fifo_next_ticket;
	long 			fifo_serving_ticket;
}	t_sim;

typedef struct s_coder
{
	int				coder_id;
	t_sim			*sim;

	long			last_compile_start_ms;
	int				compile_count;

	pthread_mutex_t	action_mutex;

	long fifo_ticket;
	int waiting_compile;
	long next_deadline_ms;
}	t_coder;

// core/monitor_checks.c
int		coder_timed_out(t_coder *c, long now, long timeout);
int		monitor_find_burned_out(t_sim *sim, long now);
int		all_compiled_enough(t_sim *sim);

// core/monitor.c
void	*monitor_routine(void *arg);

// core/sim_stop.c
int		sim_should_stop(t_sim *sim);
void	sim_request_stop(t_sim *sim);

// core/dongle.c
int		dongle_lock(t_sim *sim, int idx);
void	dongle_unlock_with_cooldown(t_sim *sim, int idx);

// core/scheduler.c
int		scheduler_wait_turn(t_coder *coder);
void	scheduler_release_turn(t_coder *coder);

// core/coder_actions.c
int		coder_do_compile(t_coder *coder, int first, int second);
void	coder_do_debug(t_coder *coder);
void	coder_do_refactor(t_coder *coder);

// app/main.c
void	sim_destroy(t_sim *sim);
void	*coder_routine(void *arg);

// utils.c
void	sleep_ms(long ms);

// log.c
void	log_state(t_sim *sim, int coder_id, const char *msg);

// time.c
long	now_ms(void);
void	coder_touch(t_coder *c);
long	timestamp_ms(t_sim *sim);
struct timespec	ms_to_abs_timespec(long abs_ms);

// sim_init.c
int		sim_init(t_sim *sim);

// parse.c
int		parse_args(t_rules *rules, int ac, char **av);

#endif
