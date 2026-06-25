/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_t_manager.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:43 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_t_manager(t_env *env)
{
	pthread_t	*thread_ids;

	thread_ids = malloc(sizeof(pthread_t) * env->nb_coders);
	if (!thread_ids)
		return (1);
	if (create_t_monitor(env) != 0 || create_t_coders(env, thread_ids) != 0)
	{
		free(thread_ids);
		return (1);
	}
	wait_t_coders(env, thread_ids);
	wait_t_monitor(env);
	free(thread_ids);
	return (0);
}
