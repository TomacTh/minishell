/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_minishell.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 16:19:48 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:05:24 by tcharvet         ###   ########.fr       */
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

t_builtin_t	is_builtin_or_not(char *str)
{
	if (!ft_strcmp(str, "env"))
		return (env_f);
	else if (!ft_strcmp(str, "export"))
		return (export_f);
	else if (!ft_strcmp(str, "exit"))
		return (exit_f);
	else if (!ft_strcmp(str, "cd"))
		return (cd_f);
	else if (!ft_strcmp(str, "unset"))
		return (unset_f);
	else if (!ft_strcmp(str, "echo"))
		return (echo_f);
	else if (!ft_strcmp(str, "pwd"))
		return (pwd_f);
	else
		return (not);
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

void	update_oldpwd(t_exp_list *oldpwd_el,
t_exp_list *pwd_el, char *buf, char *good_val)
{
	char	*fullstr;
	char	*value;

	if (oldpwd_el)
	{
		if (!good_val)
		{
			if (pwd_el && pwd_el->value)
				update_oldpwd(oldpwd_el, pwd_el, buf, pwd_el->value);
			else if (buf)
				update_oldpwd(oldpwd_el, pwd_el, buf, buf);
			return ;
		}
		else
		{
			fullstr = ft_strjoin("OLDPWD=", good_val);
			value = ft_strdup(good_val);
			if (!value || !value)
			{
				free(fullstr);
				ft_quit(1, NULL, "malloc error");
			}
			update_exp_el(fullstr, value, oldpwd_el, 0);
		}
	}
}

void	update_pwd(t_exp_list *pwd_el, char *buf)
{
	char	*fullstr;
	char	*value;

	if (pwd_el)
	{
		if (!buf)
		{
			ft_error(NULL, "updategetcwd");
			g_data.exit_status = 1;
			return ;
		}
		fullstr = ft_strjoin("PWD=", buf);
		value = ft_strdup(buf);
		if (!value || !value)
		{
			free(fullstr);
			ft_quit(1, NULL, "malloc error");
		}
		update_exp_el(fullstr, value, pwd_el, 0);
	}
}

void	chdir_check(char *path, t_exp_list *pwd_el, t_exp_list	*oldpwd_el)
{
	char	buf[4096];
	char	*ptr;
	int		error;

	error = 0;
	if (!getcwd(buf, 4095))
		error = 1;
	if (chdir(path) == -1)
	{
		ft_error(NULL, "cd");
		g_data.exit_status = 1;
		return ;
	}
	if (error)
		ptr = NULL;
	else
		ptr = buf;
	update_oldpwd(oldpwd_el, pwd_el, ptr, NULL);
	update_pwd(pwd_el, ptr);
	update_my_env(&g_data.exp_list);
}

void	handle_cd_expansions(char *path, t_exp_list	*home_el,
t_exp_list *pwd_el, t_exp_list	*oldpwd_el)
{
	char	*newpath;

	if (path[0] == '-')
	{
		if (!oldpwd_el || !oldpwd_el->value)
		{
			builtin_error("cd", NULL, "OLDPWD not set\n");
			g_data.exit_status = 1;
			return ;
		}
		newpath = oldpwd_el->value;
	}
	else
	{
		if (!home_el || !home_el->value)
		{
			builtin_error("cd", NULL, "HOME not set\n");
			g_data.exit_status = 1;
			return ;
		}
		newpath = home_el->value;
	}
	chdir_check(newpath, pwd_el, oldpwd_el);
}

void	ft_cd(char **argv, size_t count, t_exp_list **alst)
{
	t_exp_list	*home_el;
	t_exp_list	*pwd_el;
	t_exp_list	*oldpwd_el;

	home_el = search_el(*alst, "HOME");
	pwd_el = search_el(*alst, "PWD");
	oldpwd_el = search_el(*alst, "OLDPWD");
	if (count == 1 && (!home_el || !home_el->value))
	{
		builtin_error("cd", NULL, "HOME not set\n");
		g_data.exit_status = 1;
		return ;
	}
	else if (count == 1 && home_el && home_el->value)
		chdir_check(home_el->value, pwd_el, oldpwd_el);
	else if (count > 1)
	{
		if (!ft_strcmp("~", argv[1]) || !ft_strcmp("-", argv[1]))
			handle_cd_expansions(argv[1], home_el, pwd_el, oldpwd_el);
		else
			chdir_check(argv[1], pwd_el, oldpwd_el);
	}
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
