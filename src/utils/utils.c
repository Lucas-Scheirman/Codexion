/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:33:13 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000);
}

void	wait_action(t_env *env, int ms)
{
	long long	start;

	start = get_time_in_ms();
	while (get_time_in_ms() - start < ms)
	{
		if (should_stop(env))
			return ;
		usleep(200);
	}
}

void	start_clock(t_env *env)
{
	int	i;

	env->start_time = get_time_in_ms();
	i = 0;
	while (i < env->nb_coders)
	{
		env->coders[i].last_compile_start = env->start_time;
		i++;
	}
}
