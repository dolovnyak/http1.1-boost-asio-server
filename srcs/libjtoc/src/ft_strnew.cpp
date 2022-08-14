/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/27 15:27:27 by broggo            #+#    #+#             */
/*   Updated: 2018/11/27 15:27:27 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

char	*ft_strnew(size_t size)
{
	char	*arr;

	arr = (char *)malloc(size + 1);
	if (!arr)
		return (NULL);
	ft_bzero(arr, size + 1);
	return (arr);
}
