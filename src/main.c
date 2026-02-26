/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/02/26 11:15:29 by rtsubuku         ###   ########.fr       */
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

typedef struct s_sim
{
	long			start_ms;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	dongle;
}	t_sim;

typedef struct s_coder
{
	int		coder_id;
	t_sim	*sim;
}	t_coder;

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

void	*coder_routine(void *arg)
{
	int		i;
	t_coder	*coder;

	i = 0;
	coder = (t_coder *)arg;
	while (i < 9)
	{
		if (i % 3 == 0)
		{
			pthread_mutex_lock(&coder->sim->dongle);
			log_state(coder->sim, coder->coder_id, "got dongle");
			log_state(coder->sim, coder->coder_id, "is compiling");
			sleep_ms(200);
			log_state(coder->sim, coder->coder_id, "released dongle");
			pthread_mutex_unlock(&coder->sim->dongle);
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

void	sim_init(t_sim *sim)
{
	sim->start_ms = now_ms();
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->dongle, NULL);
}

void	sim_destroy(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->dongle);
}

int	main(void)
{
	t_sim		sim;
	t_coder		coders[2];
	pthread_t	th[2];
	int			i;

	sim_init(&sim);
	i = 0;
	while (i < 2)
	{
		coders[i].coder_id = i + 1;
		coders[i].sim = &sim;
		pthread_create(&th[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	i = 0;
	while (i < 2)
	{
		pthread_join(th[i], NULL);
		i++;
	}
	sim_destroy(&sim);
	return (0);
}
