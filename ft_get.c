/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 12:04:14 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:05:21 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	get_key_and_value(char *str, char **key, char **value)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != '=')
		++i;
	*key = ft_strndup(str, i);
	if (!(*key))
		ft_quit(1, 0, "malloc error");
	if (str[i])
	{
		*value = ft_strdup(&str[i + 1]);
		if (!(*value))
		{
			free(*key);
			ft_quit(1, 0, "malloc error");
		}
	}
	else
		*value = NULL;
}

t_word_list	*get_del_word(t_word_list *el, int index)
{
	int	i;

	i = 0;
	while (el)
	{
		if (el->subtype == delimiter)
		{
			if (i == index)
				return (el);
			++i;
		}
		el = el->next;
	}
	return (el);
}

void	get_key(char *key, char *str_line, size_t *i, size_t *end)
{
	size_t	y;

	y = 0;
	if (str_line[*i] == '$')
	{
		key[y++] = str_line[*i];
		++(*i);
	}
	else
	{
		while ((*i < *end) && (!is_quoted(str_line[*i]))
			&& !is_ifs(str_line[*i] && y < 4096) && str_line[*i] != '$')
		{
			key[y++] = str_line[*i];
			++(*i);
		}
	}
	key[y] = '\0';
}

int	get_expand_quote(char *buff, char *value, size_t *len)
{
	size_t	len_value;
	size_t	i;

	i = 0;
	len_value = ft_strlen(value);
	while (i < len_value && *len < 4096)
	{
		buff[(*len)++] = value[i];
		++i;
	}
	return (0);
}

int	get_expand_exitcode(char *buffer, size_t *len)
{
	char	*str;

	str = ft_udecimaltoa((int)g_data.exit_status);
	if (!str)
		ft_quit(1, NULL, "malloc error");
	get_expand_quote(buffer, str, len);
	free(str);
	return (0);
}
