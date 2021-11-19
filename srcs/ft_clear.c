/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_clear.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:09:31 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 17:09:39 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	clear_command(t_command *command)
{
	size_t	i;

	i = 0;
	while (i < command->arg_counter)
	{
		free(command->argv[i]);
		command->argv[i] = NULL;
		++i;
	}
	if (command->path)
	{
		free(command->path);
		command->path = NULL;
	}
	command->arg_counter = 0;
	command->exec_name = NULL;
	close_fd(&command->in);
	close_fd(&command->out);
	command->pipe = 0;
	command->builtin = not;
	command->arg_counter = 0;
}

void	clear_word_list(t_word_list **alst)
{
	t_word_list	*el;
	t_word_list	*temp;

	el = *alst;
	while (el)
	{
		temp = el->next;
		free(el);
		el = temp;
	}
	*alst = NULL;
}

void	clear_line(t_line *line)
{
	clear_word_list(&line->word_list);
	clear_h_doc_tab(&line->h_doc_tab, &line->h_doc_len);
	line->h_doc_i = 0;
	line->pipeline = 0;
	line->last = 0;
	if (line->str_line)
	{
		if (g_data.nonewline == 2)
			--g_data.nonewline;
		else if (g_data.nonewline == 1)
			g_data.nonewline = 0;
		free(line->str_line);
		line->str_line = NULL;
	}
	close_fd(&line->pipe_fd[0]);
	close_fd(&line->pipe_fd[1]);
}

void	clear_h_doc_tab(t_here_doc **h_doc_tab, size_t *h_doc_len)
{
	size_t		i;
	t_here_doc	*ptr;

	i = 0;
	if (*h_doc_tab)
	{
		ptr = *h_doc_tab;
		while (i < *h_doc_len)
		{
			if (ptr[i].name)
				unlink(ptr[i].name);
			free(ptr[i].name);
			free(ptr[i].delimiter);
			close_fd(&ptr[i].fd);
			++i;
		}
		free(*h_doc_tab);
		*h_doc_tab = NULL;
		*h_doc_len = 0;
	}
}

void	clear_exp_list(t_exp_list **alst)
{
	t_exp_list	*el;
	t_exp_list	*temp;

	el = *alst;
	while (el)
	{
		temp = el->next;
		free_exp_el(el);
		el = temp;
	}
	*alst = NULL;
}
