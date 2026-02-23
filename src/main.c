/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:14:38 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/02/23 10:24:49 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// main.c = 司会（スタートボタン担当）

// みんなの人数を確認して

// ルールを読み取って

// 会場を準備して

// 「よーいドン！」で動かして

// 終わったら片付ける
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

static long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
}

int	main(int ac, char **av)
{
	int		i;
	long	start;
	long	elapsed;

	(void)ac;
	(void)av;
	i = 0;
	start = now_ms();
	return (0);
}
