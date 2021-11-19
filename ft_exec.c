/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:34:38 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:36:46 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	exec_command(t_line *line, t_command *command)
{
	get_command_path(command);
	if (!command->path)
		exec_path_no_find(line, command);
	else
	{
		if (command->pipe)
			verif_pipe(line);
		command->pid = fork();
		if (!command->pid)
		{
			echo_control_seq(1);
			dup_fd(line, command);
			signal(SIGQUIT, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			execve(command->path, command->argv, g_data.myenv);
			ft_error(0, command->argv[0]);
			set_exitstatus_fromerrno();
			exit(g_data.exit_status);
		}
		verif_fork_fail(command->pid);
		close_after_fork(line, command);
		if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
			line->last = command->pid;
	}
}

void	exec_empty_command(t_line *line, t_command *command, int code)
{
	if (command->pipe)
		verif_pipe(line);
	command->pid = fork();
	if (!command->pid)
	{
		dup_fd(line, command);
		exit(code);
	}
	verif_fork_fail(command->pid);
	if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
		line->last = command->pid;
	close_after_fork(line, command);
}

void	exec_path_no_find(t_line *line, t_command *command)
{
	ft_error(NULL, command->exec_name);
	if (errno == EACCES)
		exec_empty_command(line, command, 126);
	else
		exec_empty_command(line, command, 127);
	errno = 0;
}

void	exec_builtin(t_line *line, t_command *command)
{
	if (!line->pipeline)
	{
		dup_fd(line, command);
		ft_builtin(command->argv, command->arg_counter,
			&g_data.exp_list, command->builtin);
		reset_stdin_stdout(command);
	}
	else
	{
		if (command->pipe)
			verif_pipe(line);
		command->pid = fork();
		if (!command->pid)
		{
			echo_control_seq(1);
			dup_fd(line, command);
			ft_builtin(command->argv, command->arg_counter,
				&g_data.exp_list, command->builtin);
			exit(g_data.exit_status);
		}
		verif_fork_fail(command->pid);
		close_after_fork(line, command);
		if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
			line->last = command->pid;
	}
}

void	ft_exec(t_line *line, t_command *command)
{
	if (command->builtin)
		exec_builtin(line, command);
	else if (!command->exec_name)
		exec_empty_command(line, command, 0);
	else if (!command->builtin && command->exec_name)
		exec_command(line, command);
}
