/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lscheirm <lscheirm@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 05:57:33 by lscheirm          #+#    #+#             */
/*   Updated: 2026/06/25 16:01:24 by lscheirm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	heap_swap(t_request *heap, int i, int j)
{
	t_request	tmp;

	tmp = heap[i];
	heap[i] = heap[j];
	heap[j] = tmp;
}

static void	bubble_down(t_request *heap, int n, int sched)
{
	int	i;
	int	best;
	int	child;

	i = 0;
	while (1)
	{
		best = i;
		child = i * 2 + 1;
		if (child < n && request_is_priority(&heap[child], &heap[best], sched))
			best = child;
		child = i * 2 + 2;
		if (child < n && request_is_priority(&heap[child], &heap[best], sched))
			best = child;
		if (best == i)
			break ;
		heap_swap(heap, i, best);
		i = best;
	}
}

static void	bubble_up(t_request *heap, int i, int sched)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!request_is_priority(&heap[i], &heap[parent], sched))
			break ;
		heap_swap(heap, i, parent);
		i = parent;
	}
}

void	heap_push(t_dongle *dongle, t_request req, int scheduler)
{
	dongle->heap[dongle->heap_size] = req;
	dongle->heap_size++;
	bubble_up(dongle->heap, dongle->heap_size - 1, scheduler);
}

t_request	heap_pop(t_dongle *dongle, int scheduler)
{
	t_request	top;

	top = dongle->heap[0];
	dongle->heap_size--;
	if (dongle->heap_size > 0)
	{
		dongle->heap[0] = dongle->heap[dongle->heap_size];
		bubble_down(dongle->heap, dongle->heap_size, scheduler);
	}
	return (top);
}
