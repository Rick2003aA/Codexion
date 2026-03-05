/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:22 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/05 12:37:50 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// coder.c = 参加者（働く人の台本）

// 参加者が実際にやる行動（compile/debug/refactor）

// 何回も繰り返す

// その途中でログを出す
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
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

static int	dongle_lock(t_sim *sim, int idx)
{
	t_dongle		*d;
	long			now;
	long			wait_ms;
	long			abs_deadline_ms;
	struct timespec	ts;

	d = &sim->dongles[idx];
	pthread_mutex_lock(&d->m);
	while (!sim_should_stop(sim))
	{
		now = timestamp_ms(sim);
		if (now >= d->available_at_ms)
			return (1);
		wait_ms = d->available_at_ms - now;
		abs_deadline_ms = now_ms() + wait_ms;
		ts = ms_to_abs_timespec(abs_deadline_ms);
		pthread_cond_timedwait(&d->cv, &d->m, &ts);
	}
	pthread_mutex_unlock(&d->m);
	return (0);
}

static void	dongle_unlock_with_cooldown(t_sim *sim, int idx)
{
	t_dongle 	*d;
	long		now;

	d = &sim->dongles[idx];
	now = timestamp_ms(sim);
	d->available_at_ms = now + sim->rules.dongle_cooldown_ms;
	pthread_cond_broadcast(&d->cv);
	pthread_mutex_unlock(&d->m);
}

static int	ft_max(int right, int left)
{
	int	bigger;

	if (right > left)
		bigger = right;
	else
		bigger = left;
	return (bigger);
}

static int	ft_min(int right, int left)
{
	int	smaller;

	if (right < left)
		smaller = right;
	else
		smaller = left;
	return (smaller);
}

void	*coder_routine(void *arg)
{
	int		i;
	t_coder	*coder;
	t_sim	*sim;
	int		idx;
	int		left;
	int		right;
	int		first;
	int		second;

	i = 0;
	coder = (t_coder *)arg;
	sim = coder->sim;
	pthread_mutex_lock(&coder->action_mutex);
	coder->compile_count = 0;
	pthread_mutex_unlock(&coder->action_mutex);
	idx = coder->coder_id - 1;
	left = idx;
	right = (idx + 1) % sim->dongle_count;
	first = ft_min(right, left);
	second = ft_max(right, left);
	while (!sim_should_stop(sim))
	{
		if (i % 3 == 0)
		{
			if (sim_should_stop(sim))
				break ;
			if (!dongle_lock(sim, first))
				break ;
			log_state(sim, coder->coder_id, "has taken a dongle");
			if (!dongle_lock(sim, second))
			{
				dongle_unlock_with_cooldown(sim, first);
				break ;
			}
			log_state(sim, coder->coder_id, "has taken a dongle");
			coder_touch(coder);
			log_state(sim, coder->coder_id, "is compiling");
			sleep_ms(sim->rules.time_to_compile);
			dongle_unlock_with_cooldown(sim, second);
			dongle_unlock_with_cooldown(sim, first);
			pthread_mutex_lock(&coder->action_mutex);
			coder->compile_count += 1;
			pthread_mutex_unlock(&coder->action_mutex);
		}
		else if (i % 3 == 1)
		{
			log_state(sim, coder->coder_id, "is debugging");
			sleep_ms(sim->rules.time_to_debug);
		}
		else
		{
			log_state(sim, coder->coder_id, "is refactoring");
			sleep_ms(sim->rules.time_to_refactor);
		}
		i++;
	}
	return (NULL);
}
