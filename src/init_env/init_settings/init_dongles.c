/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:46 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_env *env)
{
	int	i;

	i = 0;
	env->dongles_initialized = 0;
	while (i < env->nb_coders)
	{
		env->dongles[i].is_taken = 0;
		env->dongles[i].released_at = 0;
		env->dongles[i].next_ticket = 0;
		env->dongles[i].heap_size = 0;
		env->dongles[i].heap = malloc(sizeof(t_request) * HEAP_CAP);
		if (!env->dongles[i].heap)
			return (1);
		env->dongles_initialized++;
		i++;
	}
	return (0);
}
