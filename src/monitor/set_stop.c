/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_stop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:15:54 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	set_stop(t_env *env)
{
	int	i;

	pthread_mutex_lock(&env->stop_lock);
	env->stop = 1;
	pthread_mutex_unlock(&env->stop_lock);
	i = 0;
	while (i < env->nb_coders)
	{
		pthread_mutex_lock(&env->dongles[i].lock);
		pthread_cond_broadcast(&env->dongles[i].cond);
		pthread_mutex_unlock(&env->dongles[i].lock);
		i++;
	}
}
