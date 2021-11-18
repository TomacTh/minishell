/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 16:00:59 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 15:44:02 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"


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

void	ft_putstr_fd(char *s, int fd)
{
	if (s)
		write(fd, s, ft_strlen(s));
}

void	close_fd(int *fd)
{
	if (*fd > -1)
	{
		close(*fd);
		*fd = -1;
	}
}

int	is_op(char c)
{
	if(c == '>' || c == '<' || c == '|')
		return (1);
	return (0);
}

int	is_ifs(char c)
{
	if((c == '\t' || c == '\n') || c == ' ')
		return (1);
	return (0);
}

void	skip_ifs(char *str, int *i)
{
	while (str[*i] && is_ifs(str[*i]))
		++(*i);
}



t_word_list 	*new_word_list(t_type type, int i)
{
	t_word_list	*new;

	new = malloc(sizeof(t_word_list) * 1);
	if (!new)
		ft_quit(1, NULL, "malloc error");
	new->type = type;
	new->subtype = other;
	new->begin = i;
	new->len = 0;
	new->next = NULL;
	return (new);
}

t_word_list	*create_and_addback(t_word_list **alst, t_type type, int i)
{
	t_word_list	*new;
	t_word_list	*el;

	new = new_word_list(type, i);
	if (!*alst)
		*alst = new;
	else
	{
		el = *alst;
		while(el->next)
			el = el->next;
		el->next = new;
	}
	return (new);
}

int		new_op(char *str, int *i, t_word_list **alst)
{
	t_word_list *el;
	int			condition;
	int			same;

	el = create_and_addback(alst, op, *i);
	el->len = 1;
	if (str[*i + 1])
	{
		condition = (str[*i] == '>' || str[*i + 1] == '<');
		same = (str[*i] == str[*i + 1]);
		if (condition && same)
		{
			el->len++;
			++(*i);
		}
	}
	++(*i);
	return (0);
}


int				analyze_word(char *str, int *i)
{
	char		quote;

	while(str[*i] && !is_ifs(str[*i]) && !is_op(str[*i]))
	{
		if (str[*i] == '\'' || str[*i] == '"')
		{
			quote = str[*i];
			++(*i);
			while(str[*i] && str[*i] != quote)
				++(*i);
			if (str[*i] != quote)
				return (1);
		}
		++(*i);
	}
	return (0);
}

t_word_list		*new_word(char *str, int *i, t_word_list **alst)
{
	t_word_list	*el;
	int			begin;

	begin = *i;
	el = NULL;
	if (analyze_word(str, i))
		return (el);
	else
	{
		el = create_and_addback(alst, word, begin);
		el->len = (*i - el->begin);
		return (el);
	}
}

int 	break_in_words(char *str, int *i, t_word_list **alst)
{
	skip_ifs(str, i);
	if (!str[*i])
		return (0);
	if (is_op(str[*i]))
	{
		new_op(str, i, alst);
		return (break_in_words(str, i, alst));
	}
	else if(str[*i])
	{
		if(!new_word(str, i, alst))
			return (1);
		return (break_in_words(str, i, alst));
	}
	return (0);

}
