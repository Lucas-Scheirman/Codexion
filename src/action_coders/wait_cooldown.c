/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_cooldown.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:18:22 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cooldown_sleep(long long released_at, int cooldown)
{
	long long	remaining;

	remaining = cooldown - (get_time_in_ms() - released_at);
	if (remaining > 0)
		usleep(remaining * 1000);
}
