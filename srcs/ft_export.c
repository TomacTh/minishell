/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 00:37:58 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 00:38:45 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	display_exp_list(t_exp_list *el)
{
	while (el)
	{
		if (!el->value)
			printf("declare -x %s\n", el->key);
		else
			printf("declare -x %s=\"%s\"\n", el->key, el->value);
		el = el->next;
	}
}

void	get_env_var_len(t_exp_list **alst, size_t	*i)
{
	t_exp_list	*el;

	el = *alst;
	while (el)
	{
		if (el->value)
			++(*i);
		el = el->next;
	}
}

void	update_my_env(t_exp_list **alst)
{
	char		**my_newenv;
	t_exp_list	*el;
	size_t		i;

	el = *alst;
	i = 0;
	free (g_data.myenv);
	g_data.myenv = NULL;
	get_env_var_len(alst, &i);
	my_newenv = malloc(sizeof(char *) * (i + 1));
	if (!my_newenv)
		ft_quit(1, NULL, "malloc error");
	my_newenv[i] = NULL;
	el = *alst;
	i = 0;
	while (el)
	{
		if (el->value)
			my_newenv[i++] = el->full_str;
		el = el->next;
	}
	g_data.myenv = my_newenv;
}

void	export_loop(char **argv, size_t count, t_exp_list **alst, int *change)
{
	char		*key;
	char		*value;
	t_exp_list	*el;
	size_t		i;

	i = 0;
	while (++i < count)
	{
		get_key_and_value(argv[i], &key, &value);
		if (key_is_valid(key) && ++(*change))
		{
			el = search_el(*alst, key);
			if (!el)
				new_exp_el(argv[i], key, value, alst);
			else
			{
				free(key);
				update_exp_el(argv[i], value, el, 1);
				if (!ft_strcmp(el->key, "PATH"))
					create_update_split_path(&g_data.split_path, el->value);
			}
		}
		else
			key_is_not_valid(argv[0], argv[i], key, value);
	}
}

void	ft_export(char **argv, size_t count, t_exp_list **alst)
{
	int	change;

	change = 0;
	g_data.exit_status = 0;
	if (count == 1)
		return (display_exp_list(*alst));
	export_loop(argv, count, alst, &change);
	if (change)
		update_my_env(alst);
}
