/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   choose_dongle.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:18:09 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*choose_dongle(void *arg)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	while (should_stop(coder->env) == 0)
	{
		if (coder->dongle_left < coder->dongle_right)
		{
			first = &coder->env->dongles[coder->dongle_left];
			second = &coder->env->dongles[coder->dongle_right];
		}
		else
		{
			first = &coder->env->dongles[coder->dongle_right];
			second = &coder->env->dongles[coder->dongle_left];
		}
		action_coder(coder, first, second);
	}
	return (NULL);
}
