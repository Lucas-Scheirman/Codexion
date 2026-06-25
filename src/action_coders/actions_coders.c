/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_coders.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:18:04 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	acquire_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (!take_dongle(first, coder))
		return (0);
	log_event(coder->env, coder->id, "has taken a dongle");
	if (!take_dongle(second, coder))
		return (release_dongle(first), 0);
	log_event(coder->env, coder->id, "has taken a dongle");
	return (1);
}

static void	do_compile(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&coder->data_lock);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&coder->data_lock);
	log_event(coder->env, coder->id, "is compiling");
	wait_action(coder->env, coder->env->time_to_compile);
	pthread_mutex_lock(&coder->data_lock);
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->data_lock);
	release_dongle(first);
	release_dongle(second);
}

static void	debug_and_refactor(t_coder *coder)
{
	log_event(coder->env, coder->id, "is debugging");
	wait_action(coder->env, coder->env->time_to_debug);
	log_event(coder->env, coder->id, "is refactoring");
	wait_action(coder->env, coder->env->time_to_refactor);
}

void	action_coder(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (!acquire_dongles(coder, first, second))
		return ;
	do_compile(coder, first, second);
	debug_and_refactor(coder);
}
