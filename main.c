/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:15:39 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	int		setting[9];
	t_env	env;

	if (argc != 9)
		return (error_log_parse("Wrong number arguments"), 1);
	if (run_init_parsing(argv, setting))
		return (1);
	if (setting[1] == 0)
		return (error_log_parse(argv[1]), 1);
	if (run_init_settings(&env, setting))
		return (1);
	if (run_t_manager(&env))
	{
		run_destroy(&env);
		return (1);
	}
	run_destroy(&env);
	return (0);
}
