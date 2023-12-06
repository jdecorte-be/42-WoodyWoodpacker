/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/02 10:23:19 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/12 17:03:54 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	needlen;

	if (*needle == 0)
		return ((char *)haystack);
	if (len == 0)
		return (0);
	i = 0;
	needlen = ft_strlen(needle);
	while (haystack[i] != '\0' && i <= len - 1)
	{
		j = 0;
		while (haystack[i + j] == needle[j] && haystack[i + j] != '\0'
			&& i + j < len)
		{
			if (haystack[i + j] == '\0' || j == needlen - 1)
				return ((char *)&haystack[i]);
			j++;
		}
		i++;
	}
	return (NULL);
}
