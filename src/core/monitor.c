/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:58:42 by shinnunohis       #+#    #+#             */
/*   Updated: 2026/03/06 12:58:42 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	coder_timed_out(t_coder *c, long now, long timeout)
{
	long	last;

	pthread_mutex_lock(&c->action_mutex);
	last = c->last_compile_start_ms;
	pthread_mutex_unlock(&c->action_mutex);
	return (now - last > timeout);
}

static int	all_compiled_enough(t_sim *sim)
{
	int	i;
	int	ok;

	if (sim->rules.number_of_compiles_required <= 0)
		return (0);
	i = 0;
	while (i < sim->coder_count)
	{
		pthread_mutex_lock(&sim->coders[i].action_mutex);
		ok = (sim->coders[i].compile_count
				>= sim->rules.number_of_compiles_required);
		pthread_mutex_unlock(&sim->coders[i].action_mutex);
		if (!ok)
			return (0);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	long	now;
	int		i;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		now = timestamp_ms(sim);
		i = 0;
		while (i < sim->coder_count)
		{
			if (coder_timed_out(&sim->coders[i], now, sim->rules.time_to_burnout))
			{
				log_state(sim, sim->coders[i].coder_id, "burned out");
				sim_request_stop(sim);
				return (NULL);
			}
			i++;
		}
		if (all_compiled_enough(sim))
		{
			sim_request_stop(sim);
			printf("Mission completed");
			return (NULL);
		}
		sleep_ms(1);
	}
	return (NULL);
}
