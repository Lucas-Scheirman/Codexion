/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_action_coders.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/26 02:21:43 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*run_action_coders(void *arg)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	while (should_stop(coder->env) == 0)
	{
		choose_dongle(coder, &first, &second);
		if (!take_dongle(first, coder))
			return (NULL);
		if (!take_dongle(second, coder))
		{
			release_dongle(first);
			return (NULL);
		}
		compile(coder, first, second);
		debug_and_refactor(coder);
	}
	return (NULL);
}
