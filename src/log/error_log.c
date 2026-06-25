/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_log.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:16:27 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	error_log_parse(char *arg)
{
	fprintf(stderr, "Error: Wrong argument>>%s<<\n", arg);
	fprintf(stderr, "Usage: ./codexion <nb_coders> ");
	fprintf(stderr, "<time_to_burnout> <time_to_compile> ");
	fprintf(stderr, "<time_to_debug> <time_to_refactor> ");
	fprintf(stderr, "<nb_compiles_req> <dongle_cooldown> ");
	fprintf(stderr, "<scheduler: fifo/edf>\n");
}
