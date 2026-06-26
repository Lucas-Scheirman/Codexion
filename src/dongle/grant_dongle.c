/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grant_dongle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 03:02:13 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/26 03:02:16 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	grant_dongle(t_dongle *dongle, int scheduler, int cooldown)
{
	long long	released;

	heap_pop(dongle, scheduler);
	dongle->is_taken = 1;
	released = dongle->released_at;
	pthread_mutex_unlock(&dongle->lock);
	cooldown_sleep(released, cooldown);
}
