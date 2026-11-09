/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:15:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:15:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	sift_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (heap_compare(&heap->requests[index],
				&heap->requests[parent]) >= 0)
			break ;
		swap_requests(&heap->requests[index], &heap->requests[parent]);
		index = parent;
	}
}

static void	sift_down(t_heap *heap, int index)
{
	int	smallest;
	int	left;
	int	right;

	while (1)
	{
		smallest = index;
		left = 2 * index + 1;
		right = 2 * index + 2;
		if (left < heap->size && heap_compare(&heap->requests[left],
				&heap->requests[smallest]) < 0)
			smallest = left;
		if (right < heap->size && heap_compare(&heap->requests[right],
				&heap->requests[smallest]) < 0)
			smallest = right;
		if (smallest == index)
			break ;
		swap_requests(&heap->requests[index], &heap->requests[smallest]);
		index = smallest;
	}
}

int	heap_push(t_heap *heap, t_request req)
{
	if (!heap || heap->size >= heap->capacity)
		return (ERROR);
	heap->requests[heap->size] = req;
	heap->size++;
	sift_up(heap, heap->size - 1);
	return (OK);
}

int	heap_pop(t_heap *heap, t_request *out)
{
	if (!heap || heap->size == 0)
		return (ERROR);
	if (out)
		*out = heap->requests[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->requests[0] = heap->requests[heap->size];
		sift_down(heap, 0);
	}
	return (OK);
}
