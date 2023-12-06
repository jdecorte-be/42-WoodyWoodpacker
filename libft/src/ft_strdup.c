/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/02 12:03:41 by mbucci            #+#    #+#             */
/*   Updated: 2023/06/08 17:45:42 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*s2;
	size_t	len;
	size_t	i;

	len = ft_strlen(s);
	s2 = (char *)malloc(len + 1);
	if (!s2)
		return (NULL);
	i = -1;
	while (s[++i])
		s2[i] = s[i];
	s2[i] = 0;
	return (s2);
}
