/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 16:46:07 by broggo            #+#    #+#             */
/*   Updated: 2018/11/22 16:46:08 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	d = (unsigned char*)dst;
	s = (const unsigned char*)src;
	while (n--)
		*d++ = *s++;
	return (dst);
}
