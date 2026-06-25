/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_t_coders.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:20 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	create_t_coders(t_env *env, pthread_t *thread_ids)
{
	int	i;

	i = 0;
	while (i < env->nb_coders)
	{
		if (pthread_create(&thread_ids[i], NULL, choose_dongle,
				&env->coders[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}
