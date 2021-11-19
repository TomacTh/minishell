/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ftfunc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 23:58:32 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 23:58:55 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

char	*ft_strdup(const char *s)
{
	char	*new;
	size_t	size;

	new = NULL;
	if (s)
	{
		size = ft_strlen(s);
		new = malloc(sizeof(char) * (size + 1));
		if (!new)
			return (NULL);
		ft_memcpy(new, s, size);
		new[size] = '\0';
	}
	return (new);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (!n)
		return (0);
	while (*(unsigned char *)s1 == *(
			unsigned char *)s2 && *(unsigned char *)s1 && --n)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*(unsigned char *)s1 == *(
			unsigned char *)s2 && *(unsigned char *)s1)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void	ft_bzero(void *s, size_t n)
{
	while (n-- > 0)
		*(char *)s++ = 0;
}

void	ft_putstr_fd(char *s, int fd)
{
	if (s)
		write(fd, s, ft_strlen(s));
}
