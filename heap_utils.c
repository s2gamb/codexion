/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segambar <serge_gamb@yahoo.com>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 21:15:00 by segambar          #+#    #+#             */
/*   Updated: 2026/11/09 21:15:00 by segambar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_compare(t_request *a, t_request *b)
{
	if (a->priority_key < b->priority_key)
		return (-1);
	if (a->priority_key > b->priority_key)
		return (1);
	if (a->arrival_id < b->arrival_id)
		return (-1);
	if (a->arrival_id > b->arrival_id)
		return (1);
	return (0);
}

int	heap_init(t_heap *heap, int capacity)
{
	if (capacity <= 0)
		capacity = 4;
	heap->requests = (t_request *)malloc(sizeof(t_request) * capacity);
	if (!heap->requests)
		return (ERROR);
	heap->size = 0;
	heap->capacity = capacity;
	return (OK);
}

void	heap_destroy(t_heap *heap)
{
	if (heap->requests)
	{
		free(heap->requests);
		heap->requests = NULL;
	}
	heap->size = 0;
	heap->capacity = 0;
}

int	heap_peek(t_heap *heap, t_request *out)
{
	if (!heap || heap->size == 0)
		return (ERROR);
	if (out)
		*out = heap->requests[0];
	return (OK);
}
