/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_word.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 16:00:59 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/19 00:01:58 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

t_word_list	*create_and_addback(t_word_list **alst, t_type type, int i)
{
	t_word_list	*new;
	t_word_list	*el;

	new = new_word_list(type, i);
	if (!*alst)
		*alst = new;
	else
	{
		el = *alst;
		while (el->next)
			el = el->next;
		el->next = new;
	}
	return (new);
}

int	new_op(char *str, int *i, t_word_list **alst)
{
	t_word_list	*el;
	int			condition;
	int			same;

	el = create_and_addback(alst, op, *i);
	el->len = 1;
	if (str[*i + 1])
	{
		condition = (str[*i] == '>' || str[*i + 1] == '<');
		same = (str[*i] == str[*i + 1]);
		if (condition && same)
		{
			el->len++;
			++(*i);
		}
	}
	++(*i);
	return (0);
}

int	analyze_word(char *str, int *i)
{
	char		quote;

	while (str[*i] && !is_ifs(str[*i]) && !is_op(str[*i]))
	{
		if (str[*i] == '\'' || str[*i] == '"')
		{
			quote = str[*i];
			++(*i);
			while (str[*i] && str[*i] != quote)
				++(*i);
			if (str[*i] != quote)
				return (1);
		}
		++(*i);
	}
	return (0);
}

t_word_list	*new_word(char *str, int *i, t_word_list **alst)
{
	t_word_list	*el;
	int			begin;

	begin = *i;
	el = NULL;
	if (analyze_word(str, i))
		return (el);
	else
	{
		el = create_and_addback(alst, word, begin);
		el->len = (*i - el->begin);
		return (el);
	}
}

int	break_in_words(char *str, int *i, t_word_list **alst)
{
	while (str[*i] && is_ifs(str[*i]))
		++(*i);
	if (!str[*i])
		return (0);
	if (is_op(str[*i]))
	{
		new_op(str, i, alst);
		return (break_in_words(str, i, alst));
	}
	else if (str[*i])
	{
		if (!new_word(str, i, alst))
			return (1);
		return (break_in_words(str, i, alst));
	}
	return (0);
}
