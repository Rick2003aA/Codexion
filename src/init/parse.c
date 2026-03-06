/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:31 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/06 14:10:00 by shinnunohis      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_isdigit_str(char *str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i])
	{
		if (!('0' <= str[i] && str[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}

static int	parse_numeric_rules(t_rules *rules, char **av)
{
	rules->number_of_coders = atoi(av[1]);
	rules->time_to_burnout = atoi(av[2]);
	rules->time_to_compile = atoi(av[3]);
	rules->time_to_debug = atoi(av[4]);
	rules->time_to_refactor = atoi(av[5]);
	rules->number_of_compiles_required = atoi(av[6]);
	rules->dongle_cooldown_ms = atoi(av[7]);
	if (rules->number_of_coders < 1 || rules->time_to_burnout < 1)
		return (1);
	if (rules->time_to_compile < 1 || rules->time_to_debug < 1)
		return (1);
	if (rules->time_to_refactor < 1 || rules->number_of_compiles_required < 1)
		return (1);
	return (rules->dongle_cooldown_ms < 0);
}

static int	parse_scheduler_rule(t_rules *rules, char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
		rules->scheduler = CODEXION_FIFO;
	else if (strcmp(scheduler, "edf") == 0)
		rules->scheduler = CODEXION_EDF;
	else
		return (1);
	return (0);
}

int	parse_args(t_rules *rules, int ac, char **av)
{
	int	i;

	if (ac != 9)
		return (1);
	i = 1;
	while (i < 8)
		if (!ft_isdigit_str(av[i++]))
			return (1);
	if (parse_numeric_rules(rules, av))
		return (1);
	return (parse_scheduler_rule(rules, av[8]));
}
