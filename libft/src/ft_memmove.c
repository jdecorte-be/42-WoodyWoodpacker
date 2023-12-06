/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 12:44:07 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/12 15:25:04 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	const unsigned char	*p_src;
	unsigned char		*p_dst;

	if (src == NULL && dst == NULL)
		return (NULL);
	if (dst == src)
		return (dst);
	p_src = (unsigned char *)src;
	p_dst = (unsigned char *)dst;
	if (dst > src)
	{
		while (len-- > 0)
			*(p_dst + len) = *(p_src + len);
		return (dst);
	}
	else
	{
		while (len-- > 0)
			*p_dst++ = *p_src++;
	}
	return (dst);
}
