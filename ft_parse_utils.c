/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:59:02 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 18:00:50 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	no_expand(char *str_line)
{
	int	i;

	i = 0;
	if (!str_line[i + 1] || is_quoted(str_line[i + 1]))
		return (1);
	if (is_ifs(str_line[i]))
		return (1);
	return (0);
}

void	init_parse_f_data(
	t_parse_f *parse_data, size_t end, size_t begin, char buffer[4096])
{
	parse_data->i = begin;
	parse_data->filename = buffer;
	parse_data->len = 0;
	parse_data->locklen = 0;
	parse_data->init = 0;
	parse_data->lock_empty = 0;
	parse_data->end = end;
	parse_data->quote = '\0';
}

void	init_parse_data(t_parse *parse_data, size_t end, size_t begin)
{
	parse_data->i = begin;
	parse_data->quote = '\0';
	parse_data->end = end;
	parse_data->len = 0;
}
