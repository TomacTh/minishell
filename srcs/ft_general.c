/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_general.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 12:16:40 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:32:24 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

t_word_list	*new_word_list(t_type type, int i)
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

t_word_list	*delimit_command(
	t_line *line, t_command *command, t_word_list *cursor)
{
	t_word_list	*el;

	if (line->pipe_fd[0] != -1)
	{
		command->in = line->pipe_fd[0];
		line->pipe_fd[0] = -1;
	}
	el = cursor;
	while (el && el->subtype != pipe_t)
		el = el->next;
	if (el && el->subtype == pipe_t)
	{
		command->pipe = 1;
		el = el->next;
	}
	return (el);
}

void	new_execname(char *str, t_command *command)
{
	command->exec_name = str;
	command->argv[0] = str;
	command->builtin = is_builtin_or_not(str);
}

void	fill_and_expand_args_or_cmd(
t_line *line, t_command *cmd, t_word_list *cursor, t_word_list *next_cursor)
{
	t_word_list	*el;

	el = cursor;
	while (el != next_cursor)
	{
		if (el->subtype == other)
			get_exec_and_args(line->str_line, el, cmd);
		el = el->next;
	}
}
