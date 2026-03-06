/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:35 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 11:10:20 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	sim_init_fail(t_sim *sim, int stage, int ready_dongles)
{
	int	i;

	if (stage >= 7 && sim->dongles)
	{
		i = 0;
		while (i < ready_dongles)
		{
			pthread_cond_destroy(&sim->dongles[i].cv);
			pthread_mutex_destroy(&sim->dongles[i].m);
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}
	if (stage >= 6)
		pthread_mutex_destroy(&sim->log_mutex);
	if (stage >= 5)
		pthread_mutex_destroy(&sim->stop_mutex);
	if (stage >= 4)
		pthread_cond_destroy(&sim->sched_cv);
	if (stage >= 3)
		pthread_mutex_destroy(&sim->sched_mutex);
	if (stage >= 2 && sim->threads)
	{
		free(sim->threads);
		sim->threads = NULL;
	}
	if (stage >= 1 && sim->coders)
	{
		free(sim->coders);
		sim->coders = NULL;
	}
	return (1);
}

void	sim_destroy(t_sim *sim)
{
	int	i;

	if (!sim)
		return ;
	if (sim->dongles)
	{
		i = 0;
		while (i < sim->dongle_count)
		{
			pthread_cond_destroy(&sim->dongles[i].cv);
			pthread_mutex_destroy(&sim->dongles[i].m);
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}
	pthread_cond_destroy(&sim->sched_cv);
	pthread_mutex_destroy(&sim->sched_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}

int	sim_init(t_sim *sim)
{
	int				i;
	int				stage;
	t_coder			*coders;
	pthread_t		*th;

	stage = 0;
	sim->coders = NULL;
	sim->threads = NULL;
	sim->dongles = NULL;
	coders = malloc(sizeof(t_coder) * sim->coder_count);
	if (!coders)
		return (1);
	sim->coders = coders;
	stage = 1;
	th = malloc(sizeof(pthread_t) * sim->coder_count);
	if (!th)
		return (sim_init_fail(sim, stage, 0));
	sim->threads = th;
	stage = 2;
	sim->start_ms = now_ms();
	sim->stop = 0;
	sim->fifo_serving_ticket = 0;
	sim->fifo_next_ticket = 0;
	if (pthread_mutex_init(&sim->sched_mutex, NULL) != 0)
		return (sim_init_fail(sim, stage, 0));
	stage = 3;
	if (pthread_cond_init(&sim->sched_cv, NULL) != 0)
		return (sim_init_fail(sim, stage, 0));
	stage = 4;
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (sim_init_fail(sim, stage, 0));
	stage = 5;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (sim_init_fail(sim, stage, 0));
	stage = 6;
	sim->dongles = malloc(sizeof(t_dongle) * sim->dongle_count);
	if (!sim->dongles)
		return (sim_init_fail(sim, stage, 0));
	stage = 7;
	i = 0;
	while (i < sim->dongle_count)
	{
		if (pthread_mutex_init(&sim->dongles[i].m, NULL) != 0)
			return (sim_init_fail(sim, stage, i));
		if (pthread_cond_init(&sim->dongles[i].cv, NULL) != 0)
		{
			pthread_mutex_destroy(&sim->dongles[i].m);
			return (sim_init_fail(sim, stage, i));
		}
		sim->dongles[i].available_at_ms = 0;
		i++;
	}
	return (0);
}
