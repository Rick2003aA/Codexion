/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/02/28 15:46:03 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// main.c = 司会（スタートボタン担当）

// みんなの人数を確認して

// ルールを読み取って

// 会場を準備して

// 「よーいドン！」で動かして

// 終わったら片付ける
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct s_coder t_coder;

typedef struct s_sim
{
	long			start_ms;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	*dongles;
	int				dongle_count;
	
	int				stop;
	pthread_mutex_t	stop_mutex;
	
	t_coder			*coders;
	int				coder_count;
	long			timeout_ms;
}	t_sim;

typedef struct s_coder
{
	int				coder_id;
	t_sim			*sim;
	long			last_action_ms;
	pthread_mutex_t	action_mutex;
}	t_coder;

void	coder_touch(t_coder *c);

static long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

long	elapsed_ms(t_sim *sim)
{
	return (now_ms() - sim->start_ms);
}

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	long	elapsed;

	pthread_mutex_lock(&sim->log_mutex);
	elapsed = elapsed_ms(sim);
	printf("%ld %d %s\n", elapsed, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	sleep_ms(long ms)
{
	usleep(ms * 1000);
}

int	ft_max(int right, int left)
{
	int	bigger;

	if (right > left)
		bigger = right;
	else
		bigger = left;
	return (bigger);
}

int	ft_min(int right, int left)
{
	int	smaller;

	if (right < left)
		smaller = right;
	else
		smaller = left;
	return (smaller);
}

int	sim_should_stop(t_sim *sim)
{
	int	v;

	pthread_mutex_lock(&sim->stop_mutex);
	v = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (v);
}

void	sim_request_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

void	*coder_routine(void *arg)
{
	int		i;
	t_coder	*coder;
	int		idx;
	int		left;
	int		right;
	int		first;
	int		second;

	i = 0;
	coder = (t_coder *)arg;
	idx = coder->coder_id - 1;
	left = idx; // 後で修正が必要になるかも
	right = (idx + 1) % coder->sim->dongle_count;
	first = ft_min(right, left);
	second = ft_max(right, left);
	while (!sim_should_stop(coder->sim))
	{
		coder_touch(coder);
		if (i % 3 == 0)
		{
			pthread_mutex_lock(&coder->sim->dongles[first]);
			log_state(coder->sim, coder->coder_id, "got first dongle");
			pthread_mutex_lock(&coder->sim->dongles[second]);
			log_state(coder->sim, coder->coder_id, "got second dongle");
			log_state(coder->sim, coder->coder_id, "is compiling");
			sleep_ms(200);
			pthread_mutex_unlock(&coder->sim->dongles[second]);
			log_state(coder->sim, coder->coder_id, "released second dongle");
			pthread_mutex_unlock(&coder->sim->dongles[first]);
			log_state(coder->sim, coder->coder_id, "released first dongle");
		}
		else if (i % 3 == 1)
			log_state(coder->sim, coder->coder_id, "is debugging");
		else
		{
			log_state(coder->sim, coder->coder_id, "is refactoring");
		}
		sleep_ms(100);
		i++;
	}
	return (NULL);
}

int	sim_init(t_sim *sim)
{
	int	i;

	sim->start_ms = now_ms();
	sim->dongle_count = 2;
	sim->stop = 0;
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	sim->dongles = malloc(sizeof(pthread_mutex_t) * sim->dongle_count);
	if (!sim->dongles)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (1);
	}
	i = 0;
	while (i < sim->dongle_count)
	{
		if (pthread_mutex_init(&sim->dongles[i], NULL) != 0)
		{
			while (i > 0)
				pthread_mutex_destroy(&sim->dongles[--i]);
			free(sim->dongles);
			pthread_mutex_destroy(&sim->log_mutex);
			pthread_mutex_destroy(&sim->stop_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

void	sim_destroy(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->dongle_count)
	{
		pthread_mutex_destroy(&sim->dongles[i]);
		i++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	free(sim->dongles);
}

int	coder_timed_out(t_coder *c, long now, long timeout)
{
	long	last;

	pthread_mutex_lock(&c->action_mutex);
	last = c->last_action_ms;
	pthread_mutex_unlock(&c->action_mutex);
	return (now - last > timeout);
}

void	coder_touch(t_coder *c)
{
	pthread_mutex_lock(&c->action_mutex);
	c->last_action_ms = now_ms();
	pthread_mutex_unlock(&c->action_mutex);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	long	now;
	int		i;
	t_coder	corder;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		coder_touch(&corder);
		now = now_ms();
		i = 0;
		while (i < sim->coder_count)
		{
			if (coder_timed_out(&sim->coders[i], now, sim->timeout_ms))
			{
				log_state(sim, sim->coders[i].coder_id, "timed out -> stop");
				sim_request_stop(sim);
				break ;
			}
			i++;
		}
		sleep_ms(1);
	}
	return (NULL);
}

int	main(void)
{
	t_sim		sim;
	t_coder		coders[2];
	pthread_t	th[2];
	pthread_t	monitor_th;
	int			i;

	sim.coders = coders;
	sim.coder_count = 2;
	sim.timeout_ms = 500;
	if (sim_init(&sim))
		return (1);
	i = 0;
	while (i < 2)
	{
		coders[i].coder_id = i + 1;
		coders[i].sim = &sim;
		pthread_mutex_init(&coders[i].action_mutex, NULL);
		coders[i].last_action_ms = now_ms();
		i++;
	}
	pthread_create(&monitor_th, NULL, monitor_routine, &sim);
	i = 0;
	while (i < 2)
	{
		pthread_create(&th[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	i = 0;
	while (i < 2)
	{
		pthread_join(th[i], NULL);
		i++;
	}
	pthread_join(monitor_th, NULL);
	i = 0;
	while (i < 2)
	{
		pthread_mutex_destroy(&coders[i].action_mutex);
		i++;
	}
	sim_destroy(&sim);
	return (0);
}
