/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:32:10 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_coders(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->nb_coders)
	{
		env->coders[i].id = i + 1;
		env->coders[i].dongle_left = i;
		env->coders[i].dongle_right = (i + 1) % env->nb_coders;
		env->coders[i].env = env;
		env->coders[i].compiles_done = 0;
		i++;
	}
}
