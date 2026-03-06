/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:27:15 by shinnunohis       #+#    #+#             */
/*   Updated: 2026/03/06 13:27:15 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	edf_is_my_turn(t_coder *me)
{
	int		i;
	t_sim	*sim;
	t_coder	*best;

	i = 0;
	sim = me->sim;
	best = NULL;
	while (i < sim->coder_count)
	{
		if (sim->coders[i].waiting_compile == 1)
		{
			if (!best || sim->coders[i].next_deadline_ms < best->next_deadline_ms)
				best = &sim->coders[i];
			else if (sim->coders[i].next_deadline_ms == best->next_deadline_ms
				&& sim->coders[i].coder_id < best->coder_id)
				best = &sim->coders[i];
		}
		i++;
	}
	return (best == me);
}

int	scheduler_wait_turn(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->sched_mutex);
	if (sim->rules.scheduler == CODEXION_FIFO && coder->fifo_ticket < 0)
		coder->fifo_ticket = sim->fifo_next_ticket++;
	if (sim->rules.scheduler == CODEXION_FIFO)
		while (!sim_should_stop(sim) && coder->fifo_ticket != sim->fifo_serving_ticket)
			pthread_cond_wait(&sim->sched_cv, &sim->sched_mutex);
	else
	{
		coder->waiting_compile = 1;
		while (!sim_should_stop(sim) && !edf_is_my_turn(coder))
			pthread_cond_wait(&sim->sched_cv, &sim->sched_mutex);
	}
	if (sim_should_stop(sim))
		coder->waiting_compile = 0;
	pthread_mutex_unlock(&sim->sched_mutex);
	return (!sim_should_stop(sim));
}

void	scheduler_release_turn(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->sched_mutex);
	if (sim->rules.scheduler == CODEXION_FIFO)
	{
		if (coder->fifo_ticket == sim->fifo_serving_ticket)
			sim->fifo_serving_ticket += 1;
		coder->fifo_ticket = -1;
	}
	else
		coder->waiting_compile = 0;
	pthread_cond_broadcast(&sim->sched_cv);
	pthread_mutex_unlock(&sim->sched_mutex);
}
