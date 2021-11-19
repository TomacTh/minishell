/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handler_and_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:39:26 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:42:49 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	ctrlc_handler(int num)
{
	(void)num;
	g_data.exit_status = 1;
	if (g_data.nonewline)
	{
		if (g_data.line.str_line)
			g_data.nonewline = 0;
	}
	else
		write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	if (!g_data.line.word_list)
		rl_redisplay();
}

void	hdoc_handler(int num)
{
	(void)num;
	g_data.exit_status = 1;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	close(STDIN_FILENO);
}

void	reset_stdin_stdout(t_command *command)
{
	if (command->in != -1)
	{
		dup2(g_data.std_in_out[0], STDIN_FILENO);
		command->in = -1;
	}
	if (command->out != -1)
	{
		dup2(g_data.std_in_out[1], STDOUT_FILENO);
		command->out = -1;
	}
}

int	is_last_cmd(int pipeline, int pipe, t_builtin_t builtin)
{
	if ((pipeline && !pipe) || (!pipeline && !builtin))
		return (1);
	return (0);
}

int	contain_ifs(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (is_ifs(str[i]))
			return (1);
		++i;
	}
	return (0);
}
