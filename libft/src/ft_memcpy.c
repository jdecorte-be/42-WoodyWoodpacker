/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 12:34:59 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/15 00:20:36 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	const unsigned char	*p_src;
	unsigned char		*p_dst;

	if (src == NULL && dst == NULL)
		return (NULL);
	p_src = (unsigned char *)src;
	p_dst = (unsigned char *)dst;
	while (n-- > 0)
		*p_dst++ = *p_src++;
	return (dst);
}
