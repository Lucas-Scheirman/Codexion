/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:18:16 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	grant_dongle(t_dongle *dongle, int scheduler, int cooldown)
{
	long long	released;

	heap_pop(dongle, scheduler);
	dongle->is_taken = 1;
	released = dongle->released_at;
	pthread_mutex_unlock(&dongle->lock);
	cooldown_sleep(released, cooldown);
	return (1);
}

int	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request	req;
	int			scheduler;

	scheduler = coder->env->scheduler;
	pthread_mutex_lock(&dongle->lock);
	req.ticket = dongle->next_ticket++;
	pthread_mutex_lock(&coder->data_lock);
	req.deadline = coder->last_compile_start + coder->env->time_to_burnout;
	pthread_mutex_unlock(&coder->data_lock);
	heap_push(dongle, req, scheduler);
	while (should_stop(coder->env) == 0)
	{
		if (dongle->is_taken == 0 && dongle->heap[0].ticket == req.ticket)
			return (grant_dongle(dongle, scheduler,
					coder->env->dongle_cooldown));
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	}
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}
