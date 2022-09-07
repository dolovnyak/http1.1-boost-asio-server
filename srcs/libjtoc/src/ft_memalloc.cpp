/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memalloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/27 14:59:21 by broggo            #+#    #+#             */
/*   Updated: 2018/11/27 14:59:21 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

void	*ft_memalloc(size_t size)
{
	void	*arr;

	arr = malloc(size);
	if (!arr)
		return (NULL);
	ft_bzero(arr, size);
	return (arr);
}
