/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_minishell.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 16:19:48 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:17:38 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

/*
** This globale variable is need to access all the important vars from anywhere.
*/
t_data	g_data;

int	ft_quit(int error, char *str_error, char *suffix_err)
{
	if (error)
		ft_error(str_error, suffix_err);
	if (g_data.exp_list)
	{
		clear_exp_list(&g_data.exp_list);
		free(g_data.myenv);
		g_data.myenv = NULL;
	}
	else
		free_split(g_data.myenv, -1);
	free_split(g_data.split_path, -1);
	g_data.split_path = NULL;
	echo_control_seq(1);
	close_fd(&g_data.std_in_out[0]);
	close_fd(&g_data.std_in_out[1]);
	clear_command(&g_data.command);
	clear_line(&g_data.line);
	exit(g_data.exit_status);
}

int	treat_line(t_line *line)
{
	int	i;

	i = 0;
	if (break_in_words(line->str_line, &i, &line->word_list))
		return (1);
	else if (tag_words_and_ops(&line->word_list))
		return (1);
	else if (treat_heredocs(line, &line->word_list, &line->h_doc_len))
		return (1);
	return (0);
}

void	manage_command(t_line *line, t_command *command, t_word_list *cursor)
{
	t_word_list	*next_cursor;

	if (cursor)
	{
		next_cursor = delimit_command(line, command, cursor);
		if (perform_redirections(line, command, cursor, next_cursor))
			exec_empty_command(line, command, 1);
		else
		{
			fill_and_expand_args_or_cmd(line, command, cursor, next_cursor);
			ft_exec(line, command);
		}
		clear_command(command);
		manage_command(line, command, next_cursor);
	}
}

void	wait_line(t_data *data, t_line *line, t_command *command)
{
	while (1)
	{
		line->str_line = readline("minishell> ");
		if (!line->str_line)
			ft_exit(NULL, 1);
		if (!treat_line(line))
			manage_command(line, command, line->word_list);
		wait_last_process(line->last, &data->exit_status);
		wait_all_process();
		if (line->str_line[0])
			add_history(line->str_line);
		clear_line(line);
		echo_control_seq(0);
	}
}

int	main(int ac, char **av, char **envp)
{
	(void)av;
	if (ac != 1)
	{
		ft_putstr_fd("Error: Minishell does not support argument\n", 2);
		exit (EXIT_FAILURE);
	}
	init(envp);
	wait_line(&g_data, &g_data.line, &g_data.command);
	return (0);
}
