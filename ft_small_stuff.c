/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_small_stuff.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 00:23:15 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 00:49:08 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	wait_last_process(pid_t last, int *exit_code)
{
	int	status;
	int	signal;

	if (last > 0)
	{
		waitpid(last, &status, 0);
		if (WIFSIGNALED(status))
		{
			signal = WTERMSIG(status);
			if (signal == SIGQUIT)
				printf("Quit (core dumped)\n");
			*exit_code = signal + 128;
		}
		else
			*exit_code = WEXITSTATUS(status);
	}
}

void	wait_all_process(void)
{
	while (wait(0) != -1)
		;
	errno = 0;
}

void	echo_control_seq(int toogle)
{
	struct termios	conf;

	tcgetattr(ttyslot(), &conf);
	if (toogle)
		conf.c_lflag |= ECHOCTL;
	else
		conf.c_lflag &= ~(ECHOCTL);
	tcsetattr(ttyslot(), 0, &conf);
}

int	ft_strnchr(char c, char *str, size_t n)
{
	size_t	i;

	i = 0;
	while (str[i] && i < n)
	{
		if (str[i] == c)
			return (1);
		++i;
	}
	return (0);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*psrc;
	unsigned char	*pdst;

	pdst = (unsigned char *)dst;
	psrc = (unsigned char *)src;
	if (dst != src)
	{
		while (n--)
		{
			*pdst = *psrc;
			++pdst;
			++psrc;
		}
	}
	return (dst);
}
