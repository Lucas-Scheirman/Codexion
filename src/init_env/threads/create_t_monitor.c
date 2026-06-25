/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_t_monitor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 07:12:57 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	create_t_monitor(t_env *env)
{
	if (pthread_create(&env->monitor_thread, NULL, run_monitor_coders,
			env) != 0)
		return (1);
	return (0);
}
