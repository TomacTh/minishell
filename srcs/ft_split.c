/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 12:58:09 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 15:16:21 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	protect_malloc(void **ptr, size_t lenght)
{
	*ptr = malloc(lenght);
	if (!*ptr)
		return (0);
	return (1);
}

char	**free_split(char **strs, int len)
{
	int	i;

	if (strs)
	{
		i = 0;
		if (len > -1)
		{
			while (i < len)
			{
				free(strs[i]);
				++i;
			}
		}
		else
		{
			while (strs[i])
			{
				free(strs[i]);
				++i;
			}
		}
		free(strs);
	}
	return (0);
}

static	unsigned int	cnt_words(char *s, char c)
{
	unsigned int	count;

	count = 0;
	while (*s)
	{
		if ((*s != c))
		{
			count++;
			while (*s && *s != c)
				s++;
		}
		else
			s++;
	}
	return (count);
}

static void	fill_malloc(char *str, char *begin, char *end)
{
	while (begin < end)
	{
		*str = *begin;
		begin++;
		str++;
	}
	*str = '\0';
}

char	**ft_split(char const *s, char c)
{
	unsigned int	i;
	char			*begin;
	char			**strs;

	if (!s || !protect_malloc((void **)
			&strs, (sizeof(char *) * (cnt_words((char *)s, c) + 1))))
		return (0);
	i = 0;
	while (*(char *)s)
	{
		if ((*(char *)s != c))
		{
			begin = (char *)s;
			while (*(char *)s && (*(char *)s != c))
				s++;
			if (!protect_malloc((void **)&strs[i], (s - begin + 1)))
				return (free_split(strs, -1));
			fill_malloc(strs[i], begin, (char *)s);
			i++;
		}
		else
			s++;
	}
	strs[i] = 0;
	return (strs);
}
