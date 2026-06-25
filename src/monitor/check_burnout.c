/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_burnout.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:16 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_coder *coder)
{
	long long	elapsed;
	long long	last_start;

	pthread_mutex_lock(&coder->data_lock);
	last_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->data_lock);
	elapsed = get_time_in_ms() - last_start;
	if (elapsed > coder->env->time_to_burnout)
		return (1);
	return (0);
}
