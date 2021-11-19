/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_here_doc1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 19:22:19 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 12:33:03 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include	"ft_minishell.h"

size_t	udecimal_size(size_t n)
{
	size_t	size;

	size = 0;
	if (!n)
		size++;
	while (n)
	{
		n /= 10;
		size++;
	}
	return (size);
}

char	*ft_udecimaltoa(size_t n)
{
	char			*str;
	int				length;

	length = udecimal_size(n);
	str = malloc(length + 1);
	if (!str)
		return (NULL);
	str[length] = '\0';
	fil_str(&str[length - 1], n, "0123456789");
	return (str);
}

void	here_doc_treatement(t_here_doc *h_doc)
{
	char		*str;
	int			res;

	res = 1;
	str = NULL;
	signal(SIGINT, hdoc_handler);
	while (res)
	{
		str = readline("> ");
		if (!str || !ft_strcmp(str, h_doc->delimiter))
			res = 0;
		else
		{
			ft_putstr_fd(str, h_doc->fd);
			ft_putstr_fd("\n", h_doc->fd);
		}
		if (!str)
			g_data.nonewline = 2;
		free(str);
	}
	signal(SIGINT, ctrlc_handler);
	close(h_doc->fd);
}

int	h_doc_open_fd(t_here_doc *h_doc)
{
	if (!h_doc->name)
		ft_quit(1, NULL, "malloc error");
	if (!access(h_doc->name, F_OK))
		unlink(h_doc->name);
	h_doc->fd = open(h_doc->name, O_RDWR | O_CREAT, 0644);
	if (h_doc->fd == -1)
	{
		ft_error(NULL, h_doc->name);
		g_data.exit_status = 1;
		return (1);
	}
	return (0);
}

int	fill_heredocs(t_here_doc *h_doc_tab, size_t h_doc_len, t_line *line)
{
	size_t		i;
	struct stat	buf;

	i = 0;
	while (i < h_doc_len)
	{
		h_doc_tab[i].delimiter = get_delimiter_str(
				h_doc_tab[i].word, line->str_line);
		h_doc_tab[i].name = ft_strjoin_free_s2("/tmp/", ft_udecimaltoa(i));
		if (!h_doc_tab[i].name)
			ft_quit(1, NULL, "malloc error");
		if (h_doc_open_fd(&h_doc_tab[i]))
			return (1);
		here_doc_treatement(&h_doc_tab[i]);
		if (fstat(STDIN_FILENO, &buf) == -1)
		{
			dup2(g_data.std_in_out[0], STDIN_FILENO);
			errno = 0;
			return (1);
		}
		++i;
	}
	return (0);
}
