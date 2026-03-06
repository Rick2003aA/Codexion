/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:35 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 14:10:00 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cleanup_threads_coders(t_sim *sim)
{
	free(sim->threads);
	sim->threads = NULL;
	free(sim->coders);
	sim->coders = NULL;
}

static void	cleanup_sync_objects(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_cond_destroy(&sim->sched_cv);
	pthread_mutex_destroy(&sim->sched_mutex);
}

static int	init_sync_objects(t_sim *sim)
{
	if (pthread_mutex_init(&sim->sched_mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&sim->sched_cv, NULL) != 0)
		return (pthread_mutex_destroy(&sim->sched_mutex), 1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (pthread_cond_destroy(&sim->sched_cv),
			pthread_mutex_destroy(&sim->sched_mutex), 1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&sim->stop_mutex),
			pthread_cond_destroy(&sim->sched_cv),
			pthread_mutex_destroy(&sim->sched_mutex), 1);
	return (0);
}

static int	init_all_dongles(t_sim *sim)
{
	int	i;

	if (!(sim->dongles = malloc(sizeof(t_dongle) * sim->dongle_count)))
		return (1);
	i = -1;
	while (++i < sim->dongle_count)
	{
		if (pthread_mutex_init(&sim->dongles[i].m, NULL) != 0)
			break ;
		if (pthread_cond_init(&sim->dongles[i].cv, NULL) != 0)
			break ;
		sim->dongles[i].available_at_ms = 0;
	}
	if (i == sim->dongle_count)
		return (0);
	while (i > 0)
	{
		pthread_cond_destroy(&sim->dongles[i - 1].cv);
		pthread_mutex_destroy(&sim->dongles[i - 1].m);
		i--;
	}
	return (free(sim->dongles), sim->dongles = NULL, 1);
}

int	sim_init(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->coder_count);
	if (!sim->coders)
		return (1);
	sim->threads = malloc(sizeof(pthread_t) * sim->coder_count);
	if (!sim->threads)
		return (cleanup_threads_coders(sim), 1);
	sim->start_ms = now_ms();
	sim->stop = 0;
	sim->fifo_serving_ticket = 0;
	sim->fifo_next_ticket = 0;
	sim->dongles = NULL;
	if (init_sync_objects(sim))
		return (cleanup_threads_coders(sim), 1);
	if (init_all_dongles(sim))
		return (cleanup_sync_objects(sim), cleanup_threads_coders(sim), 1);
	return (0);
}
