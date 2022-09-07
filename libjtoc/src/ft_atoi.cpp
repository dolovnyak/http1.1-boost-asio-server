/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/24 12:55:39 by broggo            #+#    #+#             */
/*   Updated: 2018/09/24 12:55:41 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

static int		check1(char str)
{
	int i;

	i = 0;
	if (str == ' ')
		i++;
	if (str == '\n')
		i++;
	if (str == '\t')
		i++;
	if (str == '\v')
		i++;
	if (str == '\r')
		i++;
	if (str == '\f')
		i++;
	return (i);
}

int				ft_atoi(const char *str)
{
	unsigned int	numb;
	int				i;
	int				m;

	i = 0;
	m = 1;
	while (check1(*str))
		str++;
	if (*str == '-')
	{
		str++;
		m = -1;
	}
	else if (*str == '+')
		str++;
	numb = 0;
	while (*(str + i) != '\0')
	{
		if (*(str + i) > '9' || *(str + i) < '0')
			break ;
		numb = numb * 10 + *(str + i) - '0';
		i++;
	}
	return (m * numb);
}
