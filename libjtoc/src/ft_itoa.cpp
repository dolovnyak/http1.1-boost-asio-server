/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/27 17:38:16 by broggo            #+#    #+#             */
/*   Updated: 2018/11/27 17:38:17 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

static int	ft_itoa_prov(int n)
{
	unsigned int	m;
	unsigned int	i;
	int				j;

	i = 1;
	m = n;
	j = 2;
	if (n < 0)
	{
		m = -n;
		j++;
	}
	while ((m / i) >= 10)
	{
		i = i * 10;
		j++;
	}
	return (j);
}

char		*ft_itoa(int n)
{
	unsigned int	m;
	unsigned int	i;
	char			*s;

	i = 1;
	m = n;
	if (n < 0)
		m = -n;
	while ((m / i) >= 10)
		i = i * 10;
	if (!(s = (char *)malloc(sizeof(char) * ft_itoa_prov(n))))
		return (0);
	if (n < 0)
	{
		*s = '-';
		s++;
	}
	while (i)
	{
		*s = (m / i) % 10 + '0';
		s++;
		i /= 10;
	}
	*s = '\0';
	return (s - (ft_itoa_prov(n) - 1));
}
