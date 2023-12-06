/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 17:31:05 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/15 21:28:02 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t					i;
	const unsigned char		*ptr;

	i = -1;
	ptr = (const unsigned char *)s;
	while (++i < n)
		if (ptr[i] == (unsigned char)c)
			return ((unsigned char *)&ptr[i]);
	return (NULL);
}
