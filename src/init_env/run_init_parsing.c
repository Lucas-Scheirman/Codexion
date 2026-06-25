/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_init_parsing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:52 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_init_parsing(char **argv, int *setting)
{
	int	i;
	int	number;

	i = 1;
	while (i < 8)
	{
		if (is_number(argv[i]))
		{
			error_log_parse(argv[i]);
			return (1);
		}
		number = ft_atoi(argv[i]);
		if (number < 0 || is_length_number(argv[i], number) == 0)
		{
			error_log_parse(argv[i]);
			return (1);
		}
		setting[i] = number;
		i++;
	}
	if (parse_scheduler(argv[8], &setting[8]))
		return (1);
	return (0);
}
