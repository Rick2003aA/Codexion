/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/02/24 15:18:42 by rtsubuku         ###   ########.fr       */
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
	long	start_ms;
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

	elapsed = elapsed_ms(sim);
	printf("%ld %d %s\n", elapsed, coder_id, msg);
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
			log_state(coder->sim, coder->coder_id, "is compiling");
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

// int	sim_init(t_sim *sim)
// {
// 	// 開始時刻・mutex初期化など「会場準備」
// }

// void	sim_destroy(t_sim *sim)
// {
// 	// 後片付け
// }

int	main(int ac, char **av)
{
	t_sim		sim;
	t_coder		coder;
	pthread_t	th;

	(void)ac;
	(void)av;
	coder.coder_id = 1;
	coder.sim = &sim;
	sim.start_ms = now_ms();
	pthread_create(&th, NULL, coder_routine, &coder);
	pthread_join(th, NULL);
	return (0);
}
