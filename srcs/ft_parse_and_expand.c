/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse_and_expand.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:20:50 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 17:21:56 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	new_token(t_parse *p_data, t_command *command)
{
	char	*str;

	if (p_data->len)
	{
		p_data->buffer[p_data->len] = '\0';
		str = ft_strdup(p_data->buffer);
		if (!str)
			ft_quit(1, NULL, "malloc error");
		if (!command->arg_counter)
			new_execname(str, command);
		else
			command->argv[command->arg_counter] = str;
		++command->arg_counter;
		p_data->len = 0;
		p_data->quote = 0;
	}
}

void	fill_expand_space_token(t_parse *p_data, char *str, t_command *cmd)
{
	size_t	i;

	i = 0;
	while (str[i] && p_data->len < 4096)
	{
		if (is_ifs(str[i]))
			new_token(p_data, cmd);
		else
			p_data->buffer[p_data->len++] = str[i];
		++i;
	}
}

int	expansion(char *str_line, t_parse *p_data, t_command *cmd)
{
	size_t	*i;
	char	key[4096];
	int		export_exception;
	char	*value;

	i = &p_data->i;
	++(*i);
	get_key(key, str_line, &p_data->i, &p_data->end);
	if (*key == '?')
		return (get_expand_exitcode(p_data->buffer, &p_data->len));
	value = search_str(g_data.exp_list, key);
	if (!value)
		return (0);
	export_exception = (cmd->builtin == export_f
			&& ft_strnchr('=', p_data->buffer, p_data->len));
	if (p_data->quote || (!contain_ifs(value) || export_exception))
		return (get_expand_quote(p_data->buffer, value, &p_data->len));
	else if (!p_data->quote)
		fill_expand_space_token(p_data, value, cmd);
	return (0);
}

void	manage_dollar_exp(char *str_line, t_parse *p_data, t_command *cmd)
{
	size_t	*i;

	i = &p_data->i;
	if (!(p_data->quote) && is_quoted(str_line[*i + 1]))
	{
		p_data->quote = quote_value(str_line[*i + 1]);
		*i += 2;
	}
	else if ((p_data->quote == '\'' || no_expand(&str_line[*i]))
		&& p_data->len < 4096)
	{
		p_data->buffer[(p_data->len)++] = str_line[*i];
		++(*i);
	}
	else
		expansion(str_line, p_data, cmd);
}

void	get_exec_and_args(char *str_line, t_word_list *el, t_command *cmd)
{
	t_parse			p_data;
	size_t			end;

	end = (el->begin + el->len);
	init_parse_data(&p_data, end, el->begin);
	while (p_data.i < end && p_data.len < 4096)
	{
		if (!p_data.quote && is_quoted(str_line[p_data.i]))
			update_quote(&p_data.quote, str_line, &p_data.i);
		else if (p_data.quote && str_line[p_data.i] == p_data.quote)
			end_quote(&p_data.quote, &p_data.i);
		else if (str_line[p_data.i] == '$')
			manage_dollar_exp(str_line, &p_data, cmd);
		else
			add_char(p_data.buffer, str_line, &p_data.len, &p_data.i);
	}
	new_token(&p_data, cmd);
}
