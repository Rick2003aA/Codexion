/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 14:10:00 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	setup_sim(t_sim *sim, t_rules *rules, int ac, char **av)
{
	if (parse_args(rules, ac, av))
		return (1);
	sim->rules = *rules;
	sim->coder_count = rules->number_of_coders;
	sim->dongle_count = rules->number_of_coders;
	return (sim_init(sim));
}

static void	init_coders(t_sim *sim, t_rules *rules)
{
	int		i;
	t_coder	*coders;

	coders = sim->coders;
	i = 0;
	while (i < sim->coder_count)
	{
		coders[i].coder_id = i + 1;
		coders[i].sim = sim;
		pthread_mutex_init(&coders[i].action_mutex, NULL);
		coders[i].last_compile_start_ms = 0;
		coders[i].waiting_compile = 0;
		coders[i].fifo_ticket = -1;
		coders[i].next_deadline_ms = rules->time_to_burnout;
		i++;
	}
}

static void	run_simulation(t_sim *sim, pthread_t *monitor_th)
{
	int		i;
	t_coder	*coders;

	coders = sim->coders;
	pthread_create(monitor_th, NULL, monitor_routine, sim);
	i = 0;
	while (i < sim->coder_count)
	{
		pthread_create(&sim->threads[i], NULL, coder_routine, &coders[i]);
		i++;
	}
}

static void	cleanup_sim(t_sim *sim, pthread_t monitor_th)
{
	int		i;
	t_coder	*coders;

	coders = sim->coders;
	i = 0;
	while (i < sim->coder_count)
		pthread_join(sim->threads[i++], NULL);
	pthread_join(monitor_th, NULL);
	i = 0;
	while (i < sim->coder_count)
		pthread_mutex_destroy(&coders[i++].action_mutex);
	sim_destroy(sim);
	free(sim->threads);
	free(sim->coders);
}

int	main(int ac, char **av)
{
	t_sim		sim;
	t_rules		rules;
	pthread_t	monitor_th;

	if (setup_sim(&sim, &rules, ac, av))
		return (1);
	init_coders(&sim, &rules);
	run_simulation(&sim, &monitor_th);
	cleanup_sim(&sim, monitor_th);
	return (0);
}
