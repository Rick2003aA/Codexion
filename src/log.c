/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtsubuku <rtsubuku@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 08:25:28 by rtsubuku          #+#    #+#             */
/*   Updated: 2026/03/05 09:43:35 by rtsubuku         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// log.c = 放送係（アナウンス係）

// 「何秒に、誰が、何をした」を必ず1行で出す

// 同時にしゃべると混ざるから **鍵（mutex）**を使って順番にしゃべる
#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	timestamp = timestamp_ms(sim);
	printf("%ld %d %s\n", timestamp, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}
