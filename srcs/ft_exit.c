/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:15:05 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 17:44:27 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

int	ft_is_space(char c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (1);
	return (0);
}

long	ft_ltoi_exit(const char *str, int *error_code)
{
	long	num;
	long	sign;

	num = 0;
	sign = 1;
	while (ft_is_space(*(char *)str))
		str++;
	if (*(char *)str == '-' || *(char *)str == '+')
	{
		if (*(char *)str == '-')
			sign = -1;
		str++;
	}
	if (!*str)
		*error_code = 1;
	while (*(char *)str && ft_isdigit(*(char *)str))
	{
		num = num * 10 + (*(char *)str - '0');
		str++;
	}
	while (ft_is_space(*(char *)str))
		str++;
	if (*(char *)str)
		*error_code = 1;
	return (num * sign);
}

int	verify_each_digit(char *str, char *max_or_min, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		if (max_or_min[i] < str[i])
			return (1);
		++i;
	}
	return (0);
}

int	detect_long_overflow(char *str)
{
	size_t	max_len;
	size_t	i;
	char	*begin;
	int		res;

	max_len = 19;
	i = 0;
	while (str[i] && !ft_isdigit(str[i]))
		++i;
	begin = &str[i];
	while ((str[i] >= '0' && str[i] <= '9'))
		++i;
	i = &str[i] - begin;
	if (i > max_len)
		return (1);
	else if (i == max_len)
	{
		if (*str == '-')
			res = verify_each_digit(begin, "9223372036854775808", max_len);
		else
			res = verify_each_digit(begin, "9223372036854775807", max_len);
		if (res)
			return (1);
	}
	return (0);
}

void	ft_exit(char **argv, size_t count)
{
	long	num;
	int		not_valid;

	not_valid = 0;
	if (!g_data.line.pipeline)
		ft_putstr_fd("exit\n", 2);
	if (count == 1)
		ft_quit(0, NULL, NULL);
	else
	{
		num = ft_ltoi_exit(argv[1], &not_valid);
		if (not_valid || detect_long_overflow(argv[1]))
		{
			builtin_error("exit", argv[1], " :numeric argument required\n");
			g_data.exit_status = 255;
			ft_quit(0, NULL, NULL);
		}
		else if (count == 2)
		{
			g_data.exit_status = (int)num;
			ft_quit(0, NULL, NULL);
		}
		builtin_error("exit", NULL, " :too many arguments\n");
		g_data.exit_status = 1;
	}
}
