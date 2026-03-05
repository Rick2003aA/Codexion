/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:35 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/05 12:40:47 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}

int	sim_init(t_sim *sim)
{
	int			i;
	t_coder		*coders;
	t_rules		rules;
	pthread_t	*th;

	coders = malloc(sizeof(t_coder) * sim->coder_count);
	if (!coders)
		return (1);
	th = malloc(sizeof(pthread_t) * sim->coder_count);
	if (!th)
		return (free(coders), 1);
	sim->coders = coders;
	sim->threads = th;
	sim->start_ms = now_ms();
	sim->stop = 0;
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&sim->stop_mutex), 1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->dongle_count);
	if (!sim->dongles)
		return (pthread_mutex_destroy(&sim->log_mutex),
			pthread_mutex_destroy(&sim->stop_mutex), 1);
	i = 0;
	while (i < sim->dongle_count)
	{
		if (pthread_mutex_init(&sim->dongles[i], NULL) != 0)
		{
			while (i > 0)
				pthread_mutex_destroy(&sim->dongles[--i]);
			free(sim->dongles);
			sim->dongles = NULL;
			pthread_mutex_destroy(&sim->log_mutex);
			pthread_mutex_destroy(&sim->stop_mutex);
			return (1);
		}
		if (pthread_cond_init(&sim->dongles[i].cv, NULL) != 0)
		{
			pthread_mutex_destroy(&sim->dongles[i].m);
		}
		sim->dongles[i].available_at_ms = 0;
		i++;
	}
	return (0);
}
