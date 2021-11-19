/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_small_stuff2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:46:51 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 00:28:09 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	set_exitstatus_fromerrno(void)
{
	if (errno == EACCES)
		g_data.exit_status = 126;
	else if (errno == ENOENT)
		g_data.exit_status = 127;
	else
		g_data.exit_status = 1;
}

void	verif_fork_fail(pid_t pid)
{
	if (pid == -1)
	{
		ft_error(NULL, "fork");
		g_data.exit_status = 1;
	}
}

int	verif_pipe(t_line *line)
{
	if (pipe(line->pipe_fd) == -1)
	{
		line->pipe_fd[0] = -1;
		line->pipe_fd[1] = -1;
		ft_error(NULL, "pipe");
		g_data.exit_status = 1;
		return (1);
	}
	else
		return (0);
}

char	*search_str(t_exp_list *list, char *key)
{
	t_exp_list	*el;

	el = list;
	while (el)
	{
		if (!ft_strcmp(key, el->key))
			return (el->value);
		el = el->next;
	}
	return (NULL);
}


void	close_after_fork(t_line *line, t_command *command)
{
	close_fd(&line->pipe_fd[1]);
	close_fd(&command->in);
	close_fd(&command->out);
}

