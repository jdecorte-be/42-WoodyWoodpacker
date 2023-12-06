/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/04 15:25:15 by mbucci            #+#    #+#             */
/*   Updated: 2021/09/16 12:51:52 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	n_len(long n)
{
	int	count;

	count = 0;
	if (n <= 0)
	{
		count += 1;
		n *= -1;
	}
	while (n > 0)
	{
		count++;
		n /= 10;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char	*a;
	int		l;
	long	nb;

	nb = n;
	l = n_len(nb);
	a = (char *)malloc(sizeof(char) * (l + 1));
	if (!a)
		return (NULL);
	a[l--] = '\0';
	if (nb < 0)
	{
		nb *= -1;
		a[0] = '-';
	}
	if (nb == 0)
	{
		a[l] = 0 + 48;
	}
	while (nb > 0)
	{
		a[l--] = nb % 10 + 48;
		nb /= 10;
	}
	return (a);
}
