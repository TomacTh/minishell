/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtin1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:59:18 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/20 14:12:53 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

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

void	ft_builtin(
	char **argv, size_t count, t_exp_list **alst, t_builtin_t type)
{
	if (type == echo_f)
		ft_echo (argv, count);
	else if (type == exit_f)
		ft_exit (argv, count);
	else if (type == export_f)
		ft_export(argv, count, alst);
	else if (type == env_f)
		ft_env(g_data.myenv);
	else if (type == unset_f)
		ft_unset(argv, count, alst);
	else if (type == cd_f)
		ft_cd(argv, count, alst);
	else if (type == pwd_f)
		ft_pwd(alst);
}

int	ft_env(char **envp)
{
	size_t	i;

	i = 0;
	if (envp)
	{
		while (envp[i])
			printf("%s\n", envp[i++]);
	}
	g_data.exit_status = 0;
	return (0);
}

void	ft_echo(char **argv, size_t count)
{
	int		newline;
	size_t	i;

	newline = 1;
	i = 1;
	if (count && argv[1] && !ft_strcmp(argv[1], "-n"))
	{
		newline = 0;
		++i;
	}
	while (i < count)
	{
		if (argv[i])
		{
			if (count == 2 || (count == 3 && !newline))
				printf("%s", argv[i]);
			else
				printf(" %s", argv[i]);
		}
		++i;
	}
	if (newline)
		printf("\n");
	g_data.exit_status = 0;
}

void	ft_pwd(t_exp_list **alst)
{
	char		buf[4096];
	t_exp_list	*pwd_el;

	g_data.exit_status = 0;
	pwd_el = NULL;
	g_data.exit_status = 0;
	if (getcwd(buf, 4095))
	{	
		printf("%s\n", buf);
		return ;
	}
	pwd_el = search_el(*alst, "PWD");
	if (!pwd_el || !pwd_el->value)
	{	
		g_data.exit_status = 1;
		ft_error(NULL, "pwd");
	}
	else
		printf("%s\n", pwd_el->value);
}
