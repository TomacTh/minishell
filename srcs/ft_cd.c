/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 12:14:17 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:14:55 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

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
