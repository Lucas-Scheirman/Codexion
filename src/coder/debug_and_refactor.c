/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_and_refactor.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 01:50:53 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/26 01:51:17 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	debug_and_refactor(t_coder *coder)
{
	log_event(coder->env, coder->id, "is debugging");
	wait_action(coder->env, coder->env->time_to_debug);
	log_event(coder->env, coder->id, "is refactoring");
	wait_action(coder->env, coder->env->time_to_refactor);
}
