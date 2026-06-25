/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_settings.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:53 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_settings(t_env *env, int *setting)
{
	env->nb_coders = setting[1];
	env->time_to_burnout = setting[2];
	env->time_to_compile = setting[3];
	env->time_to_debug = setting[4];
	env->time_to_refactor = setting[5];
	env->nb_compiles_req = setting[6];
	env->dongle_cooldown = setting[7];
	env->scheduler = setting[8];
	env->stop = 0;
	env->dongles_initialized = 0;
	env->nb_mutex_lock = 0;
	env->nb_mutex_data = 0;
	env->nb_mutex_dongles = 0;
	env->nb_mutex_cond = 0;
	env->dongles = malloc(sizeof(t_dongle) * env->nb_coders);
	env->coders = malloc(sizeof(t_coder) * env->nb_coders);
	if (!env->dongles || !env->coders)
	{
		free(env->dongles);
		free(env->coders);
		return (1);
	}
	return (0);
}
