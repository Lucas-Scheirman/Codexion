/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_event.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 07:13:56 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_env *env, int coder_id, char *msg)
{
	pthread_mutex_lock(&env->log_lock);
	if (should_stop(env) == 0)
		printf("%lld %d %s\n", get_time_in_ms() - env->start_time,
			coder_id, msg);
	pthread_mutex_unlock(&env->log_lock);
}

void	log_burnout(t_env *env, int coder_id)
{
	pthread_mutex_lock(&env->log_lock);
	printf("%lld %d burned out\n",
		get_time_in_ms() - env->start_time, coder_id);
	pthread_mutex_unlock(&env->log_lock);
}
