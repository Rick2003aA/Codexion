/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:31 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/04 12:15:36 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// parse.c = 受付（ルールチェック係）

// 「君、名前書いてないよ（引数足りない）」

// 「それ数字じゃないよ」

// 「fifo / edf 以外はダメ」
// みたいに、入力をチェックして正しい形に整える
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

int	parse_args(t_rules *rules, int ac, char **av)
{
	int	i;

	i = 1;
	if (ac != 9)
		return (1);
	while (i < 8)
	{
		if (!ft_isdigit_str(av[i]))
			return (1);
		i++;
	}
	rules->number_of_coders = atoi(av[1]);
	if (rules->number_of_coders < 1)
		return (1);
	rules->time_to_burnout = atoi(av[2]);
	if (rules->time_to_burnout < 1)
		return (1);
	rules->time_to_compile = atoi(av[3]);
	if (rules->time_to_compile < 1)
		return (1);
	rules->time_to_debug = atoi(av[4]);
	if (rules->time_to_debug < 1)
		return (1);
	rules->time_to_refactor = atoi(av[5]);
	if (rules->time_to_refactor < 1)
		return (1);
	rules->number_of_compiles_required = atoi(av[6]);
	if (rules->number_of_compiles_required < 1)
		return (1);
	rules->dongle_cooldown_ms = atoi(av[7]);
	if (rules->dongle_cooldown_ms < 0)
		return (1);
	if (strcmp(av[8], "fifo") == 0)
		rules->scheduler = CODEXION_FIFO;
	else if (strcmp(av[8], "edf") == 0)
		rules->scheduler = CODEXION_EDF;
	else
		return (1);
	return (0);
}
