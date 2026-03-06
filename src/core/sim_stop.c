/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_stop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:27:22 by shinnunohis       #+#    #+#             */
/*   Updated: 2026/03/06 13:27:22 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_cond_broadcast(&sim->sched_cv);
	i = 0;
	while (i < sim->dongle_count)
	{
		pthread_cond_broadcast(&sim->dongles[i].cv);
		i++;
	}
}
