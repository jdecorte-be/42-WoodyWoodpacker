/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/02 13:06:25 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/15 22:23:31 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_get_start(char const *s1, char const *set)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' && ft_strchr(set, s1[i]) != NULL)
		i++;
	return (i);
}

static size_t	ft_get_end(char const *s1, char const *set)
{
	size_t	i;
	size_t	strlen;

	strlen = ft_strlen(s1);
	i = strlen;
	while (ft_strchr(set, s1[strlen - 1]) != NULL)
		strlen--;
	return (i - strlen);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;
	size_t	i;
	size_t	strlen;
	char	*str;

	if (s1 == NULL || set == NULL)
		return (NULL);
	strlen = ft_strlen(s1);
	start = ft_get_start(s1, set);
	end = ft_get_end(s1, set);
	if (start == strlen)
		str = (char *)malloc(1);
	else
		str = (char *)malloc(strlen - start - end + 1);
	if (!str)
		return (NULL);
	i = 0;
	while (s1[i] != '\0' && start + i < strlen - end)
	{
		str[i] = s1[start + i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
