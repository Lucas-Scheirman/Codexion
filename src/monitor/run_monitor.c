/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_monitor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 07:20:36 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*run_monitor(void *arg)
{
	t_env	*env;
	int		i;

	env = (t_env *)arg;
	while (should_stop(env) == 0)
	{
		i = 0;
		while (i < env->nb_coders)
		{
			if (check_burnout(&env->coders[i]))
			{
				set_stop(env);
				log_burnout(env, env->coders[i].id);
				return (NULL);
			}
			i++;
		}
		if (check_all_c_done(env))
		{
			set_stop(env);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}
