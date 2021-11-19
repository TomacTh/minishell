/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 00:15:36 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 00:19:22 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	init_here_docs(t_here_doc *h_doc_tab, size_t *here_doc_len)
{
	size_t	i;

	i = 0;
	while (i < *here_doc_len)
	{
		h_doc_tab[i].index = i;
		h_doc_tab[i].name = NULL;
		h_doc_tab[i].delimiter = NULL;
		h_doc_tab[i].fd = -1;
		h_doc_tab[i].word = get_del_word(g_data.line.word_list, i);
		++i;
	}
}

int	init_myenv(char **envp, char ***myenv)
{
	int		i;
	char	**newenv;

	i = -1;
	if (envp)
		while (envp[++i])
			;
	if (!i)
		return (1);
	newenv = malloc(sizeof(char *) * (i + 1));
	if (!newenv)
		ft_quit(1, 0, "malloc error");
	i = -1;
	while (envp[++i])
	{
		newenv[i] = ft_strdup(envp[i]);
		if (!newenv[i])
		{
			free_split(newenv, i);
			ft_quit(1, 0, "malloc error");
		}
	}
	newenv[i] = NULL;
	*myenv = newenv;
	return (0);
}


void	init_export_list(char **myenv, t_exp_list **list)
{
	int			i;
	char		*str;
	char		*key;
	char		*value;
	t_exp_list	*new;

	i = -1;
	if (myenv)
	{
		while (myenv[++i])
		{
			str = myenv[i];
			get_key_and_value(str, &key, &value);
			new = exp_list_new(str, key, value);
			add_exp_to_list(new, list);
		}
	}
}

void	init(char **envp)
{
	echo_control_seq(0);
	ft_bzero(&g_data, sizeof(t_data));
	ft_bzero(g_data.command.argv, sizeof(char *) * 4096);
	g_data.line.pipe_fd[0] = -1;
	g_data.line.pipe_fd[1] = -1;
	g_data.std_in_out[0] = dup(STDIN_FILENO);
	g_data.std_in_out[1] = dup(STDOUT_FILENO);
	g_data.command.in = -1;
	g_data.command.out = -1;
	init_myenv(envp, &g_data.myenv);
	create_update_split_path(&g_data.split_path, getenv("PATH"));
	init_export_list(g_data.myenv, &g_data.exp_list);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, ctrlc_handler);
}
