/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 13:38:20 by mbucci            #+#    #+#             */
/*   Updated: 2023/06/08 18:01:26 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, const char c)
{
	int	flag;
	int	count;
	int	i;

	flag = 0;
	count = 0;
	i = -1;
	if (c == 0)
	{
		if (!*s)
			return (0);
		return (1);
	}
	while (s[++i])
	{
		if (flag && s[i] == c)
			flag = 0;
		else if (!flag && s[i] != c)
		{
			flag = 1;
			count++;
		}
	}
	return (count);
}

static int	get_len_word(const char *s, const char c, int index)
{
	int	flag;
	int	count;
	int	i;
	int	len;

	i = -1;
	flag = 0;
	count = 0;
	len = ft_strlen(s);
	while (s[++i] && index)
	{
		if (flag && s[i] == c)
		{
			flag = 0;
			index--;
		}
		else if (!flag && s[i] != c)
			flag = 1;
	}
	while (s[i] == c)
		i++;
	while (s[i++] != c && i <= len)
		count++;
	return (count);
}

static void	fill_tab(const char *s, const char c, char **tab, int len)
{
	int	i;
	int	j;

	i = 0;
	while (s && i < len)
	{
		while (s && *s == c)
			s++;
		j = 0;
		while (s && *s != c)
		{
			tab[i][j++] = *s++;
			if (!*s)
				break ;
		}
		tab[i++][j] = '\0';
	}
}

char	**ft_split(const char *s, const char c)
{
	char	**tab;
	int		words;
	int		i;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	tab = (char **)malloc(sizeof(char *) * (words + 1));
	if (!tab)
		return (NULL);
	i = -1;
	while (++i < words)
	{
		tab[i] = (char *)malloc(get_len_word(s, c, i) + 1);
		if (!tab[i])
			return (ft_free_tab(tab));
	}
	tab[i] = NULL;
	fill_tab(s, c, tab, words);
	return (tab);
}
