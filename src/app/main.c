/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 11:12:26 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		coders[i].waiting_compile = 0;
		coders[i].fifo_ticket = -1;
		coders[i].next_deadline_ms = rules.time_to_burnout;
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
