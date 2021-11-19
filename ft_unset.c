/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 19:16:55 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 19:18:59 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	free_exp_el(t_exp_list *el)
{
	free(el->key);
	if (el->value)
		free(el->value);
	free(el->full_str);
	free(el);
}

void	key_is_not_valid(char *program_name, char *arg, char *key, char *value)
{
	builtin_error(program_name, arg, "not a valid identifier\n");
	g_data.exit_status = 1;
	if (key && value)
	{
		free(key);
		free(value);
	}
}

int	key_is_valid(char *key)
{
	size_t	i;

	i = 0;
	if (!ft_isalpha(key[i]) && key[i] != '_')
		return (0);
	++i;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		++i;
	}
	return (1);
}

void	remove_exp_el_if_exist(t_exp_list **alst, char *key)
{
	t_exp_list	*el;
	t_exp_list	*prev;

	if (*alst)
	{
		el = *alst;
		prev = NULL;
		while (el && ft_strcmp(key, el->key))
		{
			prev = el;
			el = el->next;
		}
		if (!prev)
		{
			*alst = el->next;
			free_exp_el(el);
		}
		else if (prev)
		{
			prev->next = el->next;
			free_exp_el(el);
		}
	}
}

void	ft_unset(char **argv, size_t count, t_exp_list **alst)
{
	size_t	i;
	int		change;

	change = 0;
	g_data.exit_status = 0;
	if (count == 1)
		return ;
	i = 0;
	while (++i < count)
	{
		if (!key_is_valid(argv[i]))
			key_is_not_valid(argv[0], argv[i], NULL, NULL);
		else
		{
			if (search_el(*alst, argv[i]))
			{
				if (!ft_strcmp(argv[i], "PATH"))
					create_update_split_path(&g_data.split_path, NULL);
				remove_exp_el_if_exist(alst, argv[i]);
			}
			change = 1;
		}
	}
	if (change)
		update_my_env(alst);
}
