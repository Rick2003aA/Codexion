/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:14 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 14:10:00 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
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

void	sim_destroy(t_sim *sim)
{
	if (!sim)
		return ;
	destroy_dongles(sim);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_cond_destroy(&sim->sched_cv);
	pthread_mutex_destroy(&sim->sched_mutex);
}
