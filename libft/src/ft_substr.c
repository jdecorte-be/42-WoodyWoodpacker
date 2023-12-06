/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/02 12:26:18 by mbucci            #+#    #+#             */
/*   Updated: 2021/10/01 21:07:54 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*s2;
	size_t	slen;
	size_t	i;

	if (!s)
		return (NULL);
	slen = ft_strlen(s);
	if (slen >= len)
		s2 = (char *)malloc(len + 1);
	else
		s2 = (char *)malloc(slen);
	if (!s2)
		return (NULL);
	i = 0;
	if (start <= (unsigned int)slen)
	{
		while (s[start + i] != '\0' && i < len)
		{
			s2[i] = s[start + i];
			i++;
		}
	}
	s2[i] = '\0';
	return (s2);
}
