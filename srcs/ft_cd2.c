/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/20 14:19:20 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/20 15:10:55 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	verif_str(char *str)
{
	int	countdots;

	countdots = 0;
	if (*str != '.')
		return (1);
	while (*str && (*str == '.' || *str == '/') && countdots < 3)
	{
		if (*str == '.')
			++countdots;
		else
			countdots = 0;
		str++;
	}
	if (*str || countdots > 2)
		return (1);
	return (0);
}

ssize_t	get_new_join_path(char *path, t_exp_list *pwd_el, char *buf)
{
	ssize_t	i;

	i = 0;
	while (pwd_el->value[i])
	{	
		buf[i] = pwd_el->value[i];
		++i;
	}
	if (pwd_el->value[i - 1] != '/')
		buf[i++] = '/';
	while (*path)
	{	
		buf[i] = *path;
		++path;
		++i;
	}
	buf[i] = '\0';
	return (i);
}

ssize_t	verif_if_newbufvalid(char *buf, ssize_t len)
{
	int	countdots;
	int	previous;

	countdots = 0;
	previous = 0;
	while (len > 0 && (buf[len] == '.' || buf[len] == '/'))
	{	
		if (buf[len] == '.')
			++countdots;
		else
			countdots = 0;
		if (countdots == 2)
			++previous;
		--len;
	}
	return (return_verifnewbuf(len, buf, previous));
}

int	return_check_error(ssize_t len, char *buf, char *new_joinpath, char **ptr)
{	
	if (len > -1)
	{
		new_joinpath = ft_strndup(buf, len);
		if (!new_joinpath)
			ft_quit(1, NULL, "malloc error");
		if (chdir(new_joinpath) == -1)
			len = -1;
		free(new_joinpath);
	}
	if (len == -1)
	{
		builtin_error("cd", "error",
			"getcwd: cannot access parent directories: ");
		perror(0);
		return (3);
	}
	else
	{
		*ptr = new_joinpath;
		return (2);
	}
}

int	check_error(char *path, t_exp_list *pwd_el, char **ptr)
{
	char	buf[4096];
	char	*new_joinpath;
	ssize_t	len;

	*ptr = buf;
	new_joinpath = NULL;
	if (!getcwd(buf, 4095))
	{
		if ((!pwd_el || !pwd_el->value) || verif_str(path))
			return (1);
		else
		{
			len = get_new_join_path(path, pwd_el, buf);
			len = verif_if_newbufvalid(buf, --len);
			return (return_check_error(len, buf, new_joinpath, ptr));
		}
	}
	return (1);
}
