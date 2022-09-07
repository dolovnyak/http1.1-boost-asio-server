/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: broggo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/27 14:55:18 by broggo            #+#    #+#             */
/*   Updated: 2018/09/27 14:55:21 by broggo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

char	*ft_strdup(const char *s1)
{
	char *str;

	str = (char *)malloc(sizeof(char) * (ft_strlen(s1) + 1));
	if (!str)
		return (0);
	return (ft_strcpy(str, s1));
}
