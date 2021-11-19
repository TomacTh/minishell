/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_heredoc2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 19:25:50 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 19:30:43 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

size_t	get_del_len(t_word_list *delimiter, char *line, size_t end)
{
	size_t	len;
	size_t	i;
	char	quote;

	len = 0;
	i = delimiter->begin;
	quote = '\0';
	while (i < end)
	{
		if (!quote && is_quoted(line[i]))
			update_quote(&quote, line, &i);
		else if (quote && line[i] == quote)
			end_quote(&quote, &i);
		else if (!quote && (line[i] == '$' && is_quoted(line[i + 1])))
		{
			quote = quote_value(line[i + 1]);
			i += 2;
		}
		else
		{
			++i;
			++len;
		}
	}
	return (len);
}

void	fill_del_str(t_word_list *delimiter,
	char *line, char *del_str, size_t end)
{
	size_t	y;
	size_t	i;
	char	quote;

	i = delimiter->begin;
	quote = '\0';
	y = 0;
	while (i < end)
	{
		if (!quote && is_quoted(line[i]))
			update_quote(&quote, line, &i);
		else if (quote && line[i] == quote)
			end_quote(&quote, &i);
		else if (!quote && (line[i] == '$' && is_quoted(line[i + 1])))
		{
			quote = quote_value(line[i + 1]);
			i += 2;
		}
		else
			add_char(del_str, line, &y, &i);
	}
}

char	*get_delimiter_str(t_word_list *delimiter, char *line)
{
	size_t	len;
	size_t	end;
	char	*del_str;

	end = (delimiter->begin + delimiter->len);
	len = get_del_len(delimiter, line, end);
	del_str = malloc(sizeof(len) + 1);
	if (!del_str)
		ft_quit(1, NULL, "malloc error");
	del_str[len] = '\0';
	fill_del_str(delimiter, line, del_str, end);
	return (del_str);
}

void	count_here_docs(t_word_list *el, size_t *here_doc_len)
{
	while (el)
	{
		if (el->subtype == here_doc)
			++(*here_doc_len);
		el = el->next;
	}
}

int	treat_heredocs(t_line *line, t_word_list **alst, size_t *here_doc_len)
{
	count_here_docs(*alst, here_doc_len);
	if (g_data.line.h_doc_len)
	{
		line->h_doc_tab = malloc(sizeof(t_here_doc) * (*here_doc_len));
		if (!line->h_doc_tab)
			ft_quit(1, NULL, "malloc error");
		init_here_docs(line->h_doc_tab, here_doc_len);
		if (fill_heredocs(line->h_doc_tab, line->h_doc_len, line))
			return (1);
	}
	return (0);
}
