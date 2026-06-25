/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_init_settings.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:46 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_init_settings(t_env *env, int *setting)
{
	if (init_settings(env, setting))
		return (1);
	if (init_dongles(env))
	{
		run_destroy(env);
		return (1);
	}
	init_coders(env);
	if (init_mutex_lock(env) || init_mutex_data(env) || init_mutex_dongles(env)
		|| init_mutex_cond(env))
	{
		run_destroy(env);
		return (1);
	}
	start_clock(env);
	return (0);
}
