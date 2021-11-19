/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils_parse.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:29:48 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 17:30:11 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	add_char(char *str, char *line, size_t *len, size_t *i)
{
	str[(*len)++] = line[*i];
	++(*i);
}

int	is_quoted(char c)
{
	if (c == '\'' || c == '"')
		return (1);
	return (0);
}

char	quote_value(char c)
{
	if (is_quoted(c))
		return (c);
	return ('\0');
}

void	update_quote(char *quote, char *str_line, size_t *i)
{
	*quote = str_line[*i];
	++(*i);
}

void	end_quote(char *quote, size_t *i)
{
	*quote = '\0';
	++(*i);
}
