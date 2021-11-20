/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exp_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/18 17:01:41 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/20 11:22:34 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

void	add_exp_to_list(t_exp_list *new, t_exp_list **list)
{
	t_exp_list	*prev;
	t_exp_list	*el;

	el = *list;
	prev = 0;
	while (el && (ft_strcmp(new->key, el->key) > 0))
	{
		prev = el;
		el = el->next;
	}
	if (!prev)
	{
		new->next = el;
		*list = new;
	}
	else if (prev)
	{
		prev->next = new;
		new->next = el;
	}
}

t_exp_list	*exp_list_new(char *str, char *key, char *value)
{
	t_exp_list	*new;

	new = malloc(sizeof(t_exp_list));
	if (!new)
	{
		free (str);
		free(key);
		free(value);
		ft_quit(1, 0, "malloc error");
	}
	new->full_str = str;
	new->key = key;
	new->value = value;
	new->next = NULL;
	return (new);
}

t_exp_list	*new_exp_el(char *str, char *key, char *value, t_exp_list **alst)
{
	t_exp_list	*new;
	char		*full_str;

	full_str = ft_strdup(str);
	if (!full_str)
		ft_quit(1, 0, "malloc error");
	new = exp_list_new(full_str, key, value);
	add_exp_to_list(new, alst);
	return (new);
}

t_exp_list	*search_el(t_exp_list *list, char *key)
{
	t_exp_list	*el;

	el = list;
	while (el)
	{
		if (!ft_strcmp(key, el->key))
			return (el);
		el = el->next;
	}
	return (el);
}

void	update_exp_el(char *str, char *value, t_exp_list *el, int dup)
{
	char		*full_str;

	if (dup)
	{
		full_str = ft_strdup(str);
		if (!full_str)
			ft_quit(1, 0, "malloc error");
	}
	else
		full_str = str;
	free(el->full_str);
	if (el->value)
		free(el->value);
	el->full_str = full_str;
	el->value = value;
}
