/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:50:14 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 17:55:18 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	ambigous_redirection(char *str, size_t len)
{
	ft_putstr_fd("minishell: ", 2);
	write(2, str, len);
	write(2, ": ambigous redirect\n", 20);
	return (1);
}

void	builtin_error(char *program_name, char *arg, char *error)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(program_name, 2);
	if (arg)
	{
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(arg, 2);
	}
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(error, 2);
}

void	ft_error(char *error, char *suffix_err)
{
	if (error)
		ft_putstr_fd(error, 2);
	else
	{
		ft_putstr_fd("minishell: ", 2);
		if (suffix_err)
		{
			perror(suffix_err);
			if (!ft_strcmp(suffix_err, "malloc error"))
				g_data.exit_status = 1;
		}
		else
			perror(0);
	}
}
