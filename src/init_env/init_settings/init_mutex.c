/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_mutex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:49 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_mutex_lock(t_env *env)
{
	env->nb_mutex_lock = 0;
	if (pthread_mutex_init(&env->stop_lock, NULL) != 0)
		return (1);
	env->nb_mutex_lock = 1;
	if (pthread_mutex_init(&env->log_lock, NULL) != 0)
		return (1);
	env->nb_mutex_lock = 2;
	return (0);
}

int	init_mutex_data(t_env *env)
{
	env->nb_mutex_data = 0;
	while (env->nb_mutex_data < env->nb_coders)
	{
		if (pthread_mutex_init(&env->coders[env->nb_mutex_data].data_lock,
				NULL) != 0)
			return (1);
		env->nb_mutex_data++;
	}
	return (0);
}

int	init_mutex_dongles(t_env *env)
{
	env->nb_mutex_dongles = 0;
	while (env->nb_mutex_dongles < env->nb_coders)
	{
		if (pthread_mutex_init(&env->dongles[env->nb_mutex_dongles].lock,
				NULL) != 0)
			return (1);
		env->nb_mutex_dongles++;
	}
	return (0);
}

int	init_mutex_cond(t_env *env)
{
	env->nb_mutex_cond = 0;
	while (env->nb_mutex_cond < env->nb_coders)
	{
		if (pthread_cond_init(&env->dongles[env->nb_mutex_cond].cond,
				NULL) != 0)
			return (1);
		env->nb_mutex_cond++;
	}
	return (0);
}
