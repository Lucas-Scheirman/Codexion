/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_length_number.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 06:17:32 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_length_number(char *str, int nb)
{
	int	count;
	int	number;

	if (str[0] == '+')
		str++;
	if (nb == 0)
		return (strlen(str) == 1 && str[0] == '0');
	count = 0;
	number = nb;
	while (number > 0)
	{
		number /= 10;
		count++;
	}
	return ((int)strlen(str) == count);
}
