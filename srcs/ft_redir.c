/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_redir.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:02:33 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/25 22:14:54 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	return_redir_filename(t_parse_f *p_data)
{
	if (!p_data->len & !p_data->init)
		return (1);
	else if (p_data->lock_empty && !p_data->len)
		return (1);
	else if (p_data->locklen && (p_data->locklen != p_data->len))
		return (1);
	return (0);
}

int	get_redirection_filename(char *str_line, t_word_list *el, char buffer[4096])
{
	t_parse_f		p_data;
	size_t			end;

	end = (el->begin + el->len);
	init_parse_f_data(&p_data, end, el->begin, buffer);
	while (p_data.i < end && p_data.len < 4096)
	{
		if (p_data.locklen && (p_data.locklen != p_data.len))
			return (1);
		if (!p_data.quote && is_quoted(str_line[p_data.i]))
		{
			update_quote(&p_data.quote, str_line, &p_data.i);
			p_data.init = 1;
		}
		else if (p_data.quote && str_line[p_data.i] == p_data.quote)
			end_quote(&p_data.quote, &p_data.i);
		else if (str_line[p_data.i] == '$')
			manage_dollar_redir(str_line, &p_data);
		else
			add_char(p_data.filename, str_line, &p_data.len, &p_data.i);
	}
	p_data.filename[p_data.len] = '\0';
	return (return_redir_filename(&p_data));
}

int	parse_and_open_redirs(t_line *line, t_command *command, t_word_list *el)
{
	t_word_list	*filename_el;
	char		*filename;
	char		buffer[4096];
	int			res;

	filename_el = el->next;
	res = get_redirection_filename(line->str_line, filename_el, buffer);
	if (res)
	{
		filename = &line->str_line[filename_el->begin];
		return (ambigous_redirection(filename, filename_el->len));
	}
	res = (open_filename_fd(command, buffer, el->subtype));
	if (res)
		return (1);
	return (0);
}

int	perform_redirections(
	t_line *line, t_command *command, t_word_list *cursor,
	t_word_list	*n_cursor)
{
	t_word_list	*el;
	int			newfd;

	el = cursor;
	while (el != n_cursor)
	{
		if (el->subtype < pipe_t)
		{
			if (el->subtype == here_doc)
			{
				if (open_h_doc_fd(line, command, &newfd))
					return (1);
			}
			else
			{
				if (parse_and_open_redirs(line, command, el))
					return (1);
			}
		}
		el = el->next;
	}
	return (0);
}

ssize_t	return_verifnewbuf(ssize_t len, char *buf, int previous)
{
	if (previous)
	{
		while (len >= 0 && previous)
		{
			if (buf[len] == '/' && len > 0)
				--len;
			while (len > 0 && buf[len] != '/')
				--len;
			--previous;
		}
		return (len);
	}
	return (-1);
}
