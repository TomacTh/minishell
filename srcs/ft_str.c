/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:23:24 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 23:55:29 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

char	*ft_strndup(const char *s, size_t size)
{
	char	*new;

	new = malloc((size + 1) * sizeof(char));
	if (!new)
		return (NULL);
	ft_memcpy(new, s, size);
	new[size] = '\0';
	return (new);
}

char	*ft_strjoin_free_s1(char *s1, char *s2)
{
	char	*res;

	res = ft_strjoin(s1, s2);
	free(s1);
	return (res);
}

char	*ft_strjoin_free_s2(char *s1, char *s2)
{
	char	*res;

	res = ft_strjoin(s1, s2);
	free(s2);
	return (res);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	char	*begin;

	if (!s1 || !s2)
		return (0);
	str = malloc((ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!str)
		return (0);
	begin = str;
	while (*(char *)s1)
	{
		*str = *(char *)s1;
		s1++;
		str++;
	}
	while (*(char *)s2)
	{
		*str = *(char *)s2;
		s2++;
		str++;
	}
	*str = '\0';
	return (begin);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}
