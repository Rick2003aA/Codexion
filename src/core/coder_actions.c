/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shinnunohisashiryuuichi <shinnunohisash    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:27:11 by shinnunohis       #+#    #+#             */
/*   Updated: 2026/03/06 13:27:11 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_take_dongles(t_coder *coder, int first, int second)
{
	t_sim	*sim;

	sim = coder->sim;
	if (!dongle_lock(sim, first))
		return (0);
	log_state(sim, coder->coder_id, "has taken a dongle");
	if (!dongle_lock(sim, second))
	{
		dongle_unlock_with_cooldown(sim, first);
		return (0);
	}
	log_state(sim, coder->coder_id, "has taken a dongle");
	return (1);
}

static void	coder_finish_compile(t_coder *coder, int first, int second)
{
	t_sim	*sim;

	sim = coder->sim;
	coder_touch(coder);
	pthread_mutex_lock(&sim->sched_mutex);
	coder->next_deadline_ms = timestamp_ms(sim) + sim->rules.time_to_burnout;
	pthread_mutex_unlock(&sim->sched_mutex);
	log_state(sim, coder->coder_id, "is compiling");
	sleep_ms(sim->rules.time_to_compile);
	dongle_unlock_with_cooldown(sim, second);
	dongle_unlock_with_cooldown(sim, first);
	pthread_mutex_lock(&coder->action_mutex);
	coder->compile_count += 1;
	pthread_mutex_unlock(&coder->action_mutex);
}

int	coder_do_compile(t_coder *coder, int first, int second)
{
	if (!scheduler_wait_turn(coder))
		return (0);
	if (!coder_take_dongles(coder, first, second))
	{
		scheduler_release_turn(coder);
		return (0);
	}
	coder_finish_compile(coder, first, second);
	scheduler_release_turn(coder);
	return (1);
}

void	coder_do_debug(t_coder *coder)
{
	log_state(coder->sim, coder->coder_id, "is debugging");
	sleep_ms(coder->sim->rules.time_to_debug);
}

void	coder_do_refactor(t_coder *coder)
{
	log_state(coder->sim, coder->coder_id, "is refactoring");
	sleep_ms(coder->sim->rules.time_to_refactor);
}
