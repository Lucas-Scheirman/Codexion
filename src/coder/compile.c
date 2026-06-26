/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_compile.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 01:49:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/26 01:51:38 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	compile(t_coder *coder, t_dongle *first, t_dongle *second)
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
