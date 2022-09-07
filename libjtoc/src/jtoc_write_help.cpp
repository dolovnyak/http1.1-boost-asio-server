/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jtoc_write_help.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbecker <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/19 03:54:53 by sbecker           #+#    #+#             */
/*   Updated: 2019/08/19 03:56:52 by sbecker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libjtoc.h"

int	jtoc_write_none(t_jnode *n, const int fd, const char arr)
{
	if (n->name && !arr)
	{
		write(fd, "\"", 1);
		write(fd, n->name, ft_strlen(n->name));
		write(fd, "\"", 1);
	}
	write(fd, ":NULL", 5);
	if (n->right)
		write(fd, ",", 1);
	return (FUNCTION_SUCCESS);
}

int	jtoc_write_integer(t_jnode *n, const int fd, const char arr)
{
	char	*value;

	if (!(value = ft_itoa(jtoc_get_int(n))))
		return (FUNCTION_FAILURE);
	if (n->name && !arr)
	{
		write(fd, "\"", 1);
		write(fd, n->name, ft_strlen(n->name));
		write(fd, "\":", 2);
	}
	write(fd, value, ft_strlen(value));
	if (n->right)
		write(fd, ",", 1);
	return (FUNCTION_SUCCESS);
}

int	jtoc_write_fractional(t_jnode *n, const int fd, const char arr)
{
	char	*value;

	if (!(value = jtoc_ftoa(jtoc_get_float(n), 6)))
		return (FUNCTION_FAILURE);
	if (n->name && !arr)
	{
		write(fd, "\"", 1);
		write(fd, n->name, ft_strlen(n->name));
		write(fd, "\":", 2);
	}
	write(fd, value, ft_strlen(value));
	if (n->right)
		write(fd, ",", 1);
	return (FUNCTION_SUCCESS);
}

int	jtoc_write_string(t_jnode *n, const int fd, const char arr)
{
	if (n->name && !arr)
	{
		write(fd, "\"", 1);
		write(fd, n->name, ft_strlen(n->name));
		write(fd, "\":", 2);
	}
	write(fd, "\"", 1);
	if (n->data)
		write(fd, (char *)n->data, ft_strlen((char *)n->data));
	else
		write(fd, "NULL", 4);
	write(fd, "\"", 1);
	if (n->right)
		write(fd, ",", 1);
	return (FUNCTION_SUCCESS);
}
