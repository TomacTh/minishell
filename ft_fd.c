/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:22:46 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:29:41 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	open_filename_fd(t_command *command, char *buffer, t_subtype redir)
{
	int		newfd;

	newfd = -1;
	if (redir == r_right)
		newfd = open(buffer, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	else if (redir == r_append)
		newfd = open(buffer, O_WRONLY | O_APPEND | O_CREAT, 0644);
	else if (redir == r_left)
		newfd = open(buffer, O_RDONLY, 0644);
	if (newfd == -1)
	{
		ft_error(NULL, buffer);
		return (1);
	}
	if (redir < r_left)
		switch_fd(newfd, &command->out);
	else
		switch_fd(newfd, &command->in);
	return (0);
}


void	dup_fd(t_line *line, t_command *command)
{
	if (command->in != -1)
	{
		dup2(command->in, STDIN_FILENO);
		close(command->in);
	}
	if (command->out != -1)
	{
		dup2(command->out, STDOUT_FILENO);
		close(command->out);
	}
	if (line->pipe_fd[1] != -1)
	{
		close_fd(&line->pipe_fd[0]);
		if (command->out == -1)
		{
			dup2(line->pipe_fd[1], STDOUT_FILENO);
			close_fd(&line->pipe_fd[1]);
		}
	}
	if (line->pipeline || !command->builtin)
	{
		close_fd(&g_data.std_in_out[0]);
		close_fd(&g_data.std_in_out[1]);
	}
}

void	close_fd(int *fd)
{
	if (*fd > -1)
	{
		close(*fd);
		*fd = -1;
	}
}

void	switch_fd(int newvalue, int *fd)
{
	close_fd(fd);
	*fd = newvalue;
}

int	open_h_doc_fd(t_line *line, t_command *command, int *newfd)
{
	*newfd = open(line->h_doc_tab[line->h_doc_i].name, O_RDONLY);
	if (*newfd == -1)
	{
		ft_error(NULL, line->h_doc_tab[line->h_doc_i].name);
		line->h_doc_i++;
		g_data.exit_status = 1;
		return (1);
	}
	switch_fd(*newfd, &command->in);
	line->h_doc_i++;
	return (0);
}
