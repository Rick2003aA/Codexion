/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/05 11:11:18 by rtsubuku         ###   ########.fr       */
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
		ok = (sim->coders[i].compile_count >= sim->rules.number_of_compiles_required);
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


int	main(int ac, char **av)
{
	t_sim		sim;
	t_coder		*coders;
	t_rules		rules;
	pthread_t	*th;
	pthread_t	monitor_th;
	int			i;

	if (parse_args(&rules, ac, av))
		return (1);
	sim.rules = rules;
	sim.coder_count = rules.number_of_coders;
	sim.dongle_count = rules.number_of_coders;
	if (sim_init(&sim))
		return (1);
	coders = sim.coders;
	th = sim.threads;
	i = 0;
	while (i < sim.coder_count)
	{
		coders[i].coder_id = i + 1;
		coders[i].sim = &sim;
		pthread_mutex_init(&coders[i].action_mutex, NULL);
		coders[i].last_compile_start_ms = 0;
		i++;
	}
	pthread_create(&monitor_th, NULL, monitor_routine, &sim);
	i = 0;
	while (i < sim.coder_count)
	{
		pthread_create(&th[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	i = 0;
	while (i < sim.coder_count)
	{
		pthread_join(th[i], NULL);
		i++;
	}
	pthread_join(monitor_th, NULL);
	i = 0;
	while (i < sim.coder_count)
	{
		pthread_mutex_destroy(&coders[i].action_mutex);
		i++;
	}
	sim_destroy(&sim);
	free(th);
	free(coders);
	return (0);
}
