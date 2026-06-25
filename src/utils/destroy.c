/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:15:29 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_mutex_lock(t_env *env)
{
	if (env->nb_mutex_lock == 2)
		pthread_mutex_destroy(&env->log_lock);
	if (env->nb_mutex_lock >= 1)
		pthread_mutex_destroy(&env->stop_lock);
}

void	destroy_all_mutex_data(t_env *env)
{
	while (env->nb_mutex_data > 0)
		pthread_mutex_destroy(&env->coders[--env->nb_mutex_data].data_lock);
}

void	destroy_all_mutex_dongles(t_env *env)
{
	while (env->nb_mutex_dongles > 0)
		pthread_mutex_destroy(&env->dongles[--env->nb_mutex_dongles].lock);
}

void	destroy_all_cond(t_env *env)
{
	while (env->nb_mutex_cond > 0)
		pthread_cond_destroy(&env->dongles[--env->nb_mutex_cond].cond);
}

void	run_destroy(t_env *env)
{
	int	i;

	destroy_mutex_lock(env);
	destroy_all_mutex_data(env);
	destroy_all_mutex_dongles(env);
	destroy_all_cond(env);
	i = 0;
	while (i < env->dongles_initialized)
	{
		free(env->dongles[i].heap);
		i++;
	}
	free(env->dongles);
	free(env->coders);
}
