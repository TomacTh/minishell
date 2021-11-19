/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tag.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 23:45:33 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 23:45:49 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	tag_subtype_op(t_word_list *el, char *line)
{
	if (el->len > 1)
	{
		if (!ft_strncmp(">>", &line[el->begin], 2))
			el->subtype = r_append;
		else if (!ft_strncmp("<<", &line[el->begin], 2))
			el->subtype = here_doc;
	}
	else
	{
		if (!ft_strncmp(">", &line[el->begin], 1))
			el->subtype = r_right;
		else if (!ft_strncmp("<", &line[el->begin], 1))
			el->subtype = r_left;
		else if (!ft_strncmp("|", &line[el->begin], 1))
		{
			el->subtype = pipe_t;
			if (!g_data.line.pipeline)
				g_data.line.pipeline = 1;
		}
	}
}

void	tag_ops(t_word_list **alst, char *line)
{
	t_word_list	*el;

	el = *alst;
	while (el)
	{
		if (el->type == op)
			tag_subtype_op(el, line);
		el = el->next;
	}
}

int	verif_unclosed_op(t_word_list **alst)
{
	t_word_list	*el;

	if (*alst && (*alst)->subtype == pipe_t)
		return (1);
	el = *alst;
	while (el)
	{
		if (el->type == op)
		{
			if (el->subtype < pipe_t)
			{
				if (!(el->next) || (el->next->type == op))
					return (1);
			}
			else
			{
				if (!(el->next) || (el->next->subtype == pipe_t))
					return (1);
			}
		}
		el = el->next;
	}
	return (0);
}

int	tag_words(t_word_list **alst)
{
	t_word_list	*el;
	int			redirection;

	el = *alst;
	while (el)
	{
		if (el->type == op && el->subtype < pipe_t)
		{
			redirection = el->subtype < here_doc;
			if (redirection)
				el->next->subtype = filename;
			else
				el->next->subtype = delimiter;
		}
		el = el->next;
	}
	return (0);
}

int	tag_words_and_ops(t_word_list **alst)
{
	tag_ops(alst, g_data.line.str_line);
	if (verif_unclosed_op(alst))
	{
		ft_error("minishell: syntax error\n", NULL);
		g_data.exit_status = 258;
		return (1);
	}
	tag_words(alst);
	return (0);
}
