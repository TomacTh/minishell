/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_redir_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:06:46 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:22:10 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	fill_expand_space_filename(t_parse_f *p_data, char *str)
{
	size_t	i;

	i = 0;
	while (str[i] && p_data->len < 4096)
	{
		if (is_ifs(str[i]))
		{
			if (p_data->len)
				p_data->locklen = p_data->len;
			else
				p_data->lock_empty = 1;
		}
		else
			p_data->filename[p_data->len++] = str[i];
		++i;
	}
}

int	expansion_redir(char *str_line, t_parse_f *p_data)
{
	size_t	*i;
	char	key[4096];
	char	*value;

	i = &p_data->i;
	++(*i);
	get_key(key, str_line, &p_data->i, &p_data->end);
	if (*key == '?')
		return (get_expand_exitcode(p_data->filename, &p_data->len));
	value = search_str(g_data.exp_list, key);
	if (!value)
		return (0);
	if (p_data->quote || !contain_ifs(value))
		return (get_expand_quote(p_data->filename, value, &p_data->len));
	else if (!p_data->quote)
		fill_expand_space_filename(p_data, value);
	return (0);
}

void	manage_dollar_redir(char *str_line, t_parse_f *p_data)
{
	size_t	*i;

	i = &p_data->i;
	if (!(p_data->quote) && is_quoted(str_line[*i + 1]))
	{
		p_data->quote = quote_value(str_line[*i + 1]);
		*i += 2;
		p_data->init = 1;
	}
	else if ((p_data->quote == '\'' || no_expand(&str_line[*i]))
		&& p_data->len < 4096)
	{
		p_data->filename[(p_data->len)++] = str_line[*i];
		++(*i);
	}
	else
		expansion_redir(str_line, p_data);
}
