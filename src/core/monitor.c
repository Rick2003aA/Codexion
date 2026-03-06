/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:27:00 by shinnunohis       #+#    #+#             */
/*   Updated: 2026/03/06 13:27:00 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	long	now;
	int		burned_id;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		now = timestamp_ms(sim);
		burned_id = monitor_find_burned_out(sim, now);
		if (burned_id)
		{
			log_state(sim, burned_id, "burned out");
			sim_request_stop(sim);
			return (NULL);
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
