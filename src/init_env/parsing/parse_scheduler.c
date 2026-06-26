/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_scheduler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:24 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_scheduler(char *arg, int *value)
{
	if (strcmp(arg, "fifo") == 0)
		*value = SCHED_FIFO;
	else if (strcmp(arg, "edf") == 0)
		*value = SCHED_EDF;
	else
		return (error_log_parse(arg), 1);
	return (0);
}
