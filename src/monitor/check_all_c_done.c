/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_all_c_done.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:21 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_all_c_done(t_env *env)
{
	int	i;
	int	done;

	i = 0;
	while (i < env->nb_coders)
	{
		pthread_mutex_lock(&env->coders[i].data_lock);
		done = (env->coders[i].compiles_done >= env->nb_compiles_req);
		pthread_mutex_unlock(&env->coders[i].data_lock);
		if (!done)
			return (0);
		i++;
	}
	return (1);
}
