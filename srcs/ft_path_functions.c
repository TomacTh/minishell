/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_path_functions.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 18:47:22 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:49:56 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

char	*find_good_path(char **path, char *exec_name)
{
	int		i;
	char	*path_join;
	int		permission;

	i = -1;
	permission = 0;
	if (!path)
	{
		errno = ENOENT;
		return (NULL);
	}
	while (path[++i])
	{
		path_join = ft_strjoin(path[i], exec_name);
		if (!path_join)
			ft_quit(1, NULL, "malloc error");
		if (!access(path_join, F_OK | X_OK))
			return (path_join);
		if (errno == EACCES)
			permission = 1;
		free(path_join);
	}
	if (permission)
		errno = EACCES;
	return (NULL);
}

char	*get_command_path(t_command *command)
{
	if (ft_strnchr('/', command->exec_name, ft_strlen(command->exec_name)))
	{
		command->path = ft_strdup(command->exec_name);
		if (!command->path)
			ft_quit(1, NULL, "malloc error");
	}
	else
		command->path = find_good_path(g_data.split_path, command->exec_name);
	return (command->path);
}

void	fill_new_split_path(char **new_split_path)
{
	int		i;
	char	*temp;

	i = -1;
	while (new_split_path[++i])
	{
		if (new_split_path[i][ft_strlen(new_split_path[i])] != '/')
		{
			temp = ft_strjoin(new_split_path[i], "/");
			if (!temp)
			{
				free_split(new_split_path, -1);
				ft_quit(1, 0, "malloc error");
			}
			free(new_split_path[i]);
			new_split_path[i] = temp;
		}
	}
}

void	create_update_split_path(char ***split_path, char *path)
{
	char	**new_split_path;

	if (!path || !path[0])
	{
		free_split(*split_path, -1);
		*split_path = NULL;
		return ;
	}
	new_split_path = ft_split(path, ':');
	if (!new_split_path)
		ft_quit(1, 0, "malloc error");
	free_split(*split_path, -1);
	*split_path = new_split_path;
	fill_new_split_path(*split_path);
}
