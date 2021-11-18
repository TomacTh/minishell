/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_minishell.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcharvet <tcharvet@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 16:19:48 by tcharvet          #+#    #+#             */
/*   Updated: 2021/11/18 15:15:13 by tcharvet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_minishell.h"

/*
** This globale variable is need to access all the important vars from anywhere.
*/
t_data	g_data;

void	ctrlc_handler(int num)
{
	(void)num;
	g_data.exit_status = 1;
	if (g_data.nonewline)
	{
		if (g_data.line.str_line)
			g_data.nonewline = 0;
	}
	else
		write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	if (!g_data.line.word_list)
		rl_redisplay();
}

void	hdoc_handler(int num)
{
	(void)num;
	g_data.exit_status = 1;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	close(STDIN_FILENO);
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

int	ft_quit(int error, char *str_error, char *suffix_err)
{
	if (error)
		ft_error(str_error, suffix_err);
	if (g_data.exp_list)
	{
		clear_exp_list(&g_data.exp_list);
		free(g_data.myenv);
		g_data.myenv = NULL;
	}
	else
		free_split(g_data.myenv, -1);
	free_split(g_data.split_path, -1);
	g_data.split_path = NULL;
	echo_control_seq(1);
	close_fd(&g_data.std_in_out[0]);
	close_fd(&g_data.std_in_out[1]);
	clear_command(&g_data.command);
	clear_line(&g_data.line);
	exit(g_data.exit_status);
}

char	*find_good_path(char **path, char *exec_name)
{
	int		i;
	char	*path_join;
	int		permission;

	i = -1;
	permission = 0;
	if (!path)
	{
		errno = ENOENT;
		return (NULL);
	}
	while (path[++i])
	{
		path_join = ft_strjoin(path[i], exec_name);
		if (!path_join)
			ft_quit(1, NULL, "malloc error");
		if (!access(path_join, F_OK | X_OK))
			return (path_join);
		if (errno == EACCES)
			permission = 1;
		free(path_join);
	}
	if (permission)
		errno = EACCES;
	return (NULL);
}

void	fill_new_split_path(char **new_split_path)
{
	int		i;
	char	*temp;

	i = -1;
	while (new_split_path[++i])
	{
		if (new_split_path[i][ft_strlen(new_split_path[i])] != '/')
		{
			temp = ft_strjoin(new_split_path[i], "/");
			if (!temp)
			{
				free_split(new_split_path, -1);
				ft_quit(1, 0, "malloc error");
			}
			free(new_split_path[i]);
			new_split_path[i] = temp;
		}
	}
}

void	create_update_split_path(char ***split_path, char *path)
{
	char	**new_split_path;

	if (!path || !path[0])
	{
		free_split(*split_path, -1);
		*split_path = NULL;
		return ;
	}
	new_split_path = ft_split(path, ':');
	if (!new_split_path)
		ft_quit(1, 0, "malloc error");
	free_split(*split_path, -1);
	*split_path = new_split_path;
	fill_new_split_path(*split_path);
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

char	*ft_strdup(const char *s)
{
	char	*new;
	size_t	size;

	new = NULL;
	if (s)
	{
		size = ft_strlen(s);
		new = malloc(sizeof(char) * (size + 1));
		if (!new)
			return (NULL);
		ft_memcpy(new, s, size);
		new[size] = '\0';
	}
	return (new);
}

char	*ft_strndup(const char *s, size_t size)
{
	char	*new;

	new = malloc((size + 1) * sizeof(char));
	if (!new)
		return (NULL);
	ft_memcpy(new, s, size);
	new[size] = '\0';
	return (new);
}

int	ft_env(char **envp)
{
	size_t	i;

	i = 0;
	if (envp)
	{
		while (envp[i])
			printf("%s\n", envp[i++]);
	}
	g_data.exit_status = 0;
	return (0);
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

void	get_key_and_value(char *str, char **key, char **value)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != '=')
		++i;
	*key = ft_strndup(str, i);
	if (!(*key))
		ft_quit(1, 0, "malloc error");
	if (str[i])
	{
		*value = ft_strdup(&str[i + 1]);
		if (!(*value))
		{
			free(*key);
			ft_quit(1, 0, "malloc error");
		}
	}
	else
		*value = NULL;
}

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

void	clear_command(t_command *command)
{
	size_t	i;

	i = 0;
	while (i < command->arg_counter)
	{
		free(command->argv[i]);
		command->argv[i] = NULL;
		++i;
	}
	if (command->path)
	{
		free(command->path);
		command->path = NULL;
	}
	command->arg_counter = 0;
	command->exec_name = NULL;
	close_fd(&command->in);
	close_fd(&command->out);
	command->pipe = 0;
	command->builtin = not;
	command->arg_counter = 0;
}

void	clear_word_list(t_word_list **alst)
{
	t_word_list	*el;
	t_word_list	*temp;

	el = *alst;
	while (el)
	{
		temp = el->next;
		free(el);
		el = temp;
	}
	*alst = NULL;
}

void	clear_line(t_line *line)
{
	clear_word_list(&line->word_list);
	clear_h_doc_tab(&line->h_doc_tab, &line->h_doc_len);
	line->h_doc_i = 0;
	line->pipeline = 0;
	line->last = 0;
	if (line->str_line)
	{
		if (g_data.nonewline == 2)
			--g_data.nonewline;
		else if (g_data.nonewline == 1)
			g_data.nonewline = 0;
		free(line->str_line);
		line->str_line = NULL;
	}
	close_fd(&line->pipe_fd[0]);
	close_fd(&line->pipe_fd[1]);
}

void	clear_h_doc_tab(t_here_doc **h_doc_tab, size_t *h_doc_len)
{
	size_t		i;
	t_here_doc	*ptr;

	i = 0;
	if (*h_doc_tab)
	{
		ptr = *h_doc_tab;
		while (i < *h_doc_len)
		{
			if (ptr[i].name)
				unlink(ptr[i].name);
			free(ptr[i].name);
			free(ptr[i].delimiter);
			close_fd(&ptr[i].fd);
			++i;
		}
		free(*h_doc_tab);
		*h_doc_tab = NULL;
		*h_doc_len = 0;
	}
}

void	clear_exp_list(t_exp_list **alst)
{
	t_exp_list	*el;
	t_exp_list	*temp;

	el = *alst;
	while (el)
	{
		temp = el->next;
		free_exp_el(el);
		el = temp;
	}
	*alst = NULL;
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

void	tag_subtype_op(t_word_list *el, char *line)
{
	if (el->len > 1)
	{
		if (!ft_strncmp(">>", &line[el->begin], 2))
			el->subtype = r_append;
		else if (!ft_strncmp("<<", &line[el->begin], 2))
			el->subtype = here_doc;
	}
	else
	{
		if (!ft_strncmp(">", &line[el->begin], 1))
			el->subtype = r_right;
		else if (!ft_strncmp("<", &line[el->begin], 1))
			el->subtype = r_left;
		else if (!ft_strncmp("|", &line[el->begin], 1))
		{
			el->subtype = pipe_t;
			if (!g_data.line.pipeline)
				g_data.line.pipeline = 1;
		}
	}
}

void	tag_ops(t_word_list **alst, char *line)
{
	t_word_list	*el;

	el = *alst;
	while (el)
	{
		if (el->type == op)
			tag_subtype_op(el, line);
		el = el->next;
	}
}

int	verif_unclosed_op(t_word_list **alst)
{
	t_word_list	*el;

	if (*alst && (*alst)->subtype == pipe_t)
		return (1);
	el = *alst;
	while (el)
	{
		if (el->type == op)
		{
			if (el->subtype < pipe_t)
			{
				if (!(el->next) || (el->next->type == op))
					return (1);
			}
			else
			{
				if (!(el->next) || (el->next->subtype == pipe_t))
					return (1);
			}
		}
		el = el->next;
	}
	return (0);
}

int	tag_words(t_word_list **alst)
{
	t_word_list	*el;
	int			redirection;

	el = *alst;
	while (el)
	{
		if (el->type == op && el->subtype < pipe_t)
		{
			redirection = el->subtype < here_doc;
			if (redirection)
				el->next->subtype = filename;
			else
				el->next->subtype = delimiter;
		}
		el = el->next;
	}
	return (0);
}

int	tag_words_and_ops(t_word_list **alst)
{
	tag_ops(alst, g_data.line.str_line);
	if (verif_unclosed_op(alst))
	{
		ft_error("minishell: syntax error\n", NULL);
		g_data.exit_status = 258;
		return (1);
	}
	tag_words(alst);
	return (0);
}

void	count_here_docs(t_word_list *el, size_t *here_doc_len)
{
	while (el)
	{
		if (el->subtype == here_doc)
			++(*here_doc_len);
		el = el->next;
	}
}

t_word_list	*get_del_word(t_word_list *el, int index)
{
	int	i;

	i = 0;
	while (el)
	{
		if (el->subtype == delimiter)
		{
			if (i == index)
				return (el);
			++i;
		}
		el = el->next;
	}
	return (el);
}

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

int	is_quoted(char c)
{
	if (c == '\'' || c == '"')
		return (1);
	return (0);
}

char	quote_value(char c)
{
	if (is_quoted(c))
		return (c);
	return ('\0');
}

void	update_quote(char *quote, char *str_line, size_t *i)
{
	*quote = str_line[*i];
	++(*i);
}

void	end_quote(char *quote, size_t *i)
{
	*quote = '\0';
	++(*i);
}

size_t	get_del_len(t_word_list *delimiter, char *line, size_t end)
{
	size_t	len;
	size_t	i;
	char	quote;

	len = 0;
	i = delimiter->begin;
	quote = '\0';
	while (i < end)
	{
		if (!quote && is_quoted(line[i]))
			update_quote(&quote, line, &i);
		else if (quote && line[i] == quote)
			end_quote(&quote, &i);
		else if (!quote && (line[i] == '$' && is_quoted(line[i + 1])))
		{
			quote = quote_value(line[i + 1]);
			i += 2;
		}
		else
		{
			++i;
			++len;
		}
	}
	return (len);
}

void	add_char(char *str, char *line, size_t *len, size_t *i)
{
	str[(*len)++] = line[*i];
	++(*i);
}

void	fill_del_str(t_word_list *delimiter,
	char *line, char *del_str, size_t end)
{
	size_t	y;
	size_t	i;
	char	quote;

	i = delimiter->begin;
	quote = '\0';
	y = 0;
	while (i < end)
	{
		if (!quote && is_quoted(line[i]))
			update_quote(&quote, line, &i);
		else if (quote && line[i] == quote)
			end_quote(&quote, &i);
		else if (!quote && (line[i] == '$' && is_quoted(line[i + 1])))
		{
			quote = quote_value(line[i + 1]);
			i += 2;
		}
		else
			add_char(del_str, line, &y, &i);
	}
}

char	*get_delimiter_str(t_word_list *delimiter, char *line)
{
	size_t	len;
	size_t	end;
	char	*del_str;

	end = (delimiter->begin + delimiter->len);
	len = get_del_len(delimiter, line, end);
	del_str = malloc(sizeof(len) + 1);
	if (!del_str)
		ft_quit(1, NULL, "malloc error");
	del_str[len] = '\0';
	fill_del_str(delimiter, line, del_str, end);
	return (del_str);
}

void	fil_str(char *str, size_t num, char *base)
{
	while (num >= (size_t)10)
	{
		*str = base[num % 10];
		--str;
		num /= 10;
	}
	*str = base[num % 10];
}

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

int	treat_heredocs(t_line *line, t_word_list **alst, size_t *here_doc_len)
{
	count_here_docs(*alst, here_doc_len);
	if (g_data.line.h_doc_len)
	{
		line->h_doc_tab = malloc(sizeof(t_here_doc) * (*here_doc_len));
		if (!line->h_doc_tab)
			ft_quit(1, NULL, "malloc error");
		init_here_docs(line->h_doc_tab, here_doc_len);
		if (fill_heredocs(line->h_doc_tab, line->h_doc_len, line))
			return (1);
	}
	return (0);
}

int	treat_line(t_line *line)
{
	int	i;

	i = 0;
	if (break_in_words(line->str_line, &i, &line->word_list))
		return (1);
	else if (tag_words_and_ops(&line->word_list))
		return (1);
	else if (treat_heredocs(line, &line->word_list, &line->h_doc_len))
		return (1);
	return (0);
}

t_word_list	*delimit_command(
	t_line *line, t_command *command, t_word_list *cursor)
{
	t_word_list	*el;

	if (line->pipe_fd[0] != -1)
	{
		command->in = line->pipe_fd[0];
		line->pipe_fd[0] = -1;
	}
	el = cursor;
	while (el && el->subtype != pipe_t)
		el = el->next;
	if (el && el->subtype == pipe_t)
	{
		command->pipe = 1;
		el = el->next;
	}
	return (el);
}

void	switch_fd(int newvalue, int *fd)
{
	close_fd(fd);
	*fd = newvalue;
}

int	open_h_doc_fd(t_line *line, t_command *command, int *newfd)
{
	*newfd = open(line->h_doc_tab[line->h_doc_i].name, O_RDONLY);
	if (*newfd == -1)
	{
		ft_error(NULL, line->h_doc_tab[line->h_doc_i].name);
		line->h_doc_i++;
		g_data.exit_status = 1;
		return (1);
	}
	switch_fd(*newfd, &command->in);
	line->h_doc_i++;
	return (0);
}

void	get_key(char *key, char *str_line, size_t *i, size_t *end)
{
	size_t	y;

	y = 0;
	if (str_line[*i] == '$')
	{
		key[y++] = str_line[*i];
		++(*i);
	}
	else
	{
		while ((*i < *end) && (!is_quoted(str_line[*i]))
			&& !is_ifs(str_line[*i] && y < 4096) && str_line[*i] != '$')
		{
			key[y++] = str_line[*i];
			++(*i);
		}
	}
	key[y] = '\0';
}

int	get_expand_quote(char *buff, char *value, size_t *len)
{
	size_t	len_value;
	size_t	i;

	i = 0;
	len_value = ft_strlen(value);
	while (i < len_value && *len < 4096)
	{
		buff[(*len)++] = value[i];
		++i;
	}
	return (0);
}

int	get_expand_exitcode(char *buffer, size_t *len)
{
	char	*str;

	str = ft_udecimaltoa((int)g_data.exit_status);
	if (!str)
		ft_quit(1, NULL, "malloc error");
	get_expand_quote(buffer, str, len);
	free(str);
	return (0);
}

int	contain_ifs(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (is_ifs(str[i]))
			return (1);
		++i;
	}
	return (0);
}

void	fill_expand_space_filename(t_parse_f *p_data, char *str)
{
	size_t	i;

	i = 0;
	while (str[i] && p_data->len < 4096)
	{
		if (is_ifs(str[i]))
		{
			if (p_data->len)
				p_data->locklen = p_data->len;
			else
				p_data->lock_empty = 1;
		}
		else
			p_data->filename[p_data->len++] = str[i];
		++i;
	}
}

char	*search_str(t_exp_list *list, char *key)
{
	t_exp_list	*el;

	el = list;
	while (el)
	{
		if (!ft_strcmp(key, el->key))
			return (el->value);
		el = el->next;
	}
	return (NULL);
}

int	expansion_redir(char *str_line, t_parse_f *p_data)
{
	size_t	*i;
	char	key[4096];
	char	*value;

	i = &p_data->i;
	++(*i);
	get_key(key, str_line, &p_data->i, &p_data->end);
	if (*key == '?')
		return (get_expand_exitcode(p_data->filename, &p_data->len));
	value = search_str(g_data.exp_list, key);
	if (!value)
		return (0);
	if (p_data->quote || !contain_ifs(value))
		return (get_expand_quote(p_data->filename, value, &p_data->len));
	else if (!p_data->quote)
		fill_expand_space_filename(p_data, value);
	return (0);
}

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

void	manage_dollar_redir(char *str_line, t_parse_f *p_data)
{
	size_t	*i;

	i = &p_data->i;
	if (!(p_data->quote) && is_quoted(str_line[*i + 1]))
	{
		p_data->quote = quote_value(str_line[*i + 1]);
		*i += 2;
		p_data->init = 1;
	}
	else if ((p_data->quote == '\'' || no_expand(&str_line[*i]))
		&& p_data->len < 4096)
	{
		p_data->filename[(p_data->len)++] = str_line[*i];
		++(*i);
	}
	else
		expansion_redir(str_line, p_data);
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

int	return_redir_filename(t_parse_f *p_data)
{
	if (!p_data->len & !p_data->init)
		return (1);
	else if (p_data->lock_empty && !p_data->len)
		return (1);
	else if (p_data->locklen && (p_data->locklen != p_data->len))
		return (1);
	return (0);
}

int	get_redirection_filename(char *str_line, t_word_list *el, char buffer[4096])
{
	t_parse_f		p_data;
	size_t			end;

	end = (el->begin + el->len);
	init_parse_f_data(&p_data, end, el->begin, buffer);
	while (p_data.i < end && p_data.len < 4096)
	{
		if (p_data.locklen && (p_data.locklen != p_data.len))
			return (1);
		if (!p_data.quote && is_quoted(str_line[p_data.i]))
		{
			update_quote(&p_data.quote, str_line, &p_data.i);
			p_data.init = 1;
		}
		else if (p_data.quote && str_line[p_data.i] == p_data.quote)
			end_quote(&p_data.quote, &p_data.i);
		else if (str_line[p_data.i] == '$')
			manage_dollar_redir(str_line, &p_data);
		else
			add_char(p_data.filename, str_line, &p_data.len, &p_data.i);
	}
	p_data.filename[p_data.len] = '\0';
	return (return_redir_filename(&p_data));
}

int	ambigous_redirection(char *str, size_t len)
{
	ft_putstr_fd("minishell: ", 2);
	write(2, str, len);
	write(2, ": ambigous redirect\n", 20);
	return (1);
}

int	open_filename_fd(t_command *command, char *buffer, t_subtype redir)
{
	int		newfd;

	newfd = -1;
	if (redir == r_right)
		newfd = open(buffer, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	else if (redir == r_append)
		newfd = open(buffer, O_WRONLY | O_APPEND | O_CREAT, 0644);
	else if (redir == r_left)
		newfd = open(buffer, O_RDONLY, 0644);
	if (newfd == -1)
	{
		ft_error(NULL, buffer);
		return (1);
	}
	if (redir < r_left)
		switch_fd(newfd, &command->out);
	else
		switch_fd(newfd, &command->in);
	return (0);
}

int	parse_and_open_redirs(t_line *line, t_command *command, t_word_list *el)
{
	t_word_list	*filename_el;
	char		*filename;
	char		buffer[4096];
	int			res;

	filename_el = el->next;
	res = get_redirection_filename(line->str_line, filename_el, buffer);
	if (res)
	{
		filename = &line->str_line[filename_el->begin];
		return (ambigous_redirection(filename, filename_el->len));
	}
	res = (open_filename_fd(command, buffer, el->subtype));
	if (res)
		return (1);
	return (0);
}

int	perform_redirections(
	t_line *line, t_command *command, t_word_list *cursor,
	t_word_list	*n_cursor)
{
	t_word_list	*el;
	int			newfd;

	el = cursor;
	while (el != n_cursor)
	{
		if (el->subtype < pipe_t)
		{
			if (el->subtype == here_doc)
			{
				if (open_h_doc_fd(line, command, &newfd))
					return (1);
			}
			else
			{
				if (parse_and_open_redirs(line, command, el))
					return (1);
			}
		}
		el = el->next;
	}
	return (0);
}

int	is_last_cmd(int pipeline, int pipe, t_builtin_t builtin)
{
	if ((pipeline && !pipe) || (!pipeline && !builtin))
		return (1);
	return (0);
}

void	dup_fd(t_line *line, t_command *command)
{
	if (command->in != -1)
	{
		dup2(command->in, STDIN_FILENO);
		close(command->in);
	}
	if (command->out != -1)
	{
		dup2(command->out, STDOUT_FILENO);
		close(command->out);
	}
	if (line->pipe_fd[1] != -1)
	{
		close_fd(&line->pipe_fd[0]);
		if (command->out == -1)
		{
			dup2(line->pipe_fd[1], STDOUT_FILENO);
			close_fd(&line->pipe_fd[1]);
		}
	}
	if (line->pipeline || !command->builtin)
	{
		close_fd(&g_data.std_in_out[0]);
		close_fd(&g_data.std_in_out[1]);
	}
}

void	close_after_fork(t_line *line, t_command *command)
{
	close_fd(&line->pipe_fd[1]);
	close_fd(&command->in);
	close_fd(&command->out);
}

void	exec_empty_command(t_line *line, t_command *command, int code)
{
	if (command->pipe)
		verif_pipe(line);
	command->pid = fork();
	if (!command->pid)
	{
		dup_fd(line, command);
		exit(code);
	}
	verif_fork_fail(command->pid);
	if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
		line->last = command->pid;
	close_after_fork(line, command);
}

void	init_parse_data(t_parse *parse_data, size_t end, size_t begin)
{
	parse_data->i = begin;
	parse_data->quote = '\0';
	parse_data->end = end;
	parse_data->len = 0;
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

t_builtin_t	is_builtin_or_not(char *str)
{
	if (!ft_strcmp(str, "env"))
		return (env_f);
	else if (!ft_strcmp(str, "export"))
		return (export_f);
	else if (!ft_strcmp(str, "exit"))
		return (exit_f);
	else if (!ft_strcmp(str, "cd"))
		return (cd_f);
	else if (!ft_strcmp(str, "unset"))
		return (unset_f);
	else if (!ft_strcmp(str, "echo"))
		return (echo_f);
	else if (!ft_strcmp(str, "pwd"))
		return (pwd_f);
	else
		return (not);
}

void	new_execname(char *str, t_command *command)
{
	command->exec_name = str;
	command->argv[0] = str;
	command->builtin = is_builtin_or_not(str);
}

void	new_token(t_parse *p_data, t_command *command)
{
	char	*str;

	if (p_data->len)
	{
		p_data->buffer[p_data->len] = '\0';
		str = ft_strdup(p_data->buffer);
		if (!str)
			ft_quit(1, NULL, "malloc error");
		if (!command->arg_counter)
			new_execname(str, command);
		else
			command->argv[command->arg_counter] = str;
		++command->arg_counter;
		p_data->len = 0;
		p_data->quote = 0;
	}
}

void	fill_expand_space_token(t_parse *p_data, char *str, t_command *cmd)
{
	size_t	i;

	i = 0;
	while (str[i] && p_data->len < 4096)
	{
		if (is_ifs(str[i]))
			new_token(p_data, cmd);
		else
			p_data->buffer[p_data->len++] = str[i];
		++i;
	}
}

int	expansion(char *str_line, t_parse *p_data, t_command *cmd)
{
	size_t	*i;
	char	key[4096];
	int		export_exception;
	char	*value;

	i = &p_data->i;
	++(*i);
	get_key(key, str_line, &p_data->i, &p_data->end);
	if (*key == '?')
		return (get_expand_exitcode(p_data->buffer, &p_data->len));
	value = search_str(g_data.exp_list, key);
	if (!value)
		return (0);
	export_exception = (cmd->builtin == export_f
			&& ft_strnchr('=', p_data->buffer, p_data->len));
	if (p_data->quote || (!contain_ifs(value) || export_exception))
		return (get_expand_quote(p_data->buffer, value, &p_data->len));
	else if (!p_data->quote)
		fill_expand_space_token(p_data, value, cmd);
	return (0);
}

void	manage_dollar_exp(char *str_line, t_parse *p_data, t_command *cmd)
{
	size_t	*i;

	i = &p_data->i;
	if (!(p_data->quote) && is_quoted(str_line[*i + 1]))
	{
		p_data->quote = quote_value(str_line[*i + 1]);
		*i += 2;
	}
	else if ((p_data->quote == '\'' || no_expand(&str_line[*i]))
		&& p_data->len < 4096)
	{
		p_data->buffer[(p_data->len)++] = str_line[*i];
		++(*i);
	}
	else
		expansion(str_line, p_data, cmd);
}

void	get_exec_and_args(char *str_line, t_word_list *el, t_command *cmd)
{
	t_parse			p_data;
	size_t			end;

	end = (el->begin + el->len);
	init_parse_data(&p_data, end, el->begin);
	while (p_data.i < end && p_data.len < 4096)
	{
		if (!p_data.quote && is_quoted(str_line[p_data.i]))
			update_quote(&p_data.quote, str_line, &p_data.i);
		else if (p_data.quote && str_line[p_data.i] == p_data.quote)
			end_quote(&p_data.quote, &p_data.i);
		else if (str_line[p_data.i] == '$')
			manage_dollar_exp(str_line, &p_data, cmd);
		else
			add_char(p_data.buffer, str_line, &p_data.len, &p_data.i);
	}
	new_token(&p_data, cmd);
}

void	fill_and_expand_args_or_cmd(
t_line *line, t_command *cmd, t_word_list *cursor, t_word_list *next_cursor)
{
	t_word_list	*el;

	el = cursor;
	while (el != next_cursor)
	{
		if (el->subtype == other)
			get_exec_and_args(line->str_line, el, cmd);
		el = el->next;
	}
}

void	exec_path_no_find(t_line *line, t_command *command)
{
	ft_error(NULL, command->exec_name);
	if (errno == EACCES)
		exec_empty_command(line, command, 126);
	else
		exec_empty_command(line, command, 127);
	errno = 0;
}

char	*get_command_path(t_command *command)
{
	if (ft_strnchr('/', command->exec_name, ft_strlen(command->exec_name)))
	{
		command->path = ft_strdup(command->exec_name);
		if (!command->path)
			ft_quit(1, NULL, "malloc error");
	}
	else
		command->path = find_good_path(g_data.split_path, command->exec_name);
	return (command->path);
}

void	set_exitstatus_fromerrno(void)
{
	if (errno == EACCES)
		g_data.exit_status = 126;
	else if (errno == ENOENT)
		g_data.exit_status = 127;
	else
		g_data.exit_status = 1;
}

void	verif_fork_fail(pid_t pid)
{
	if (pid == -1)
	{
		ft_error(NULL, "fork");
		g_data.exit_status = 1;
	}
}

void	exec_command(t_line *line, t_command *command)
{
	get_command_path(command);
	if (!command->path)
		exec_path_no_find(line, command);
	else
	{
		if (command->pipe)
			verif_pipe(line);
		command->pid = fork();
		if (!command->pid)
		{
			echo_control_seq(1);
			dup_fd(line, command);
			signal(SIGQUIT, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			execve(command->path, command->argv, g_data.myenv);
			ft_error(0, command->argv[0]);
			set_exitstatus_fromerrno();
			exit(g_data.exit_status);
		}
		verif_fork_fail(command->pid);
		close_after_fork(line, command);
		if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
			line->last = command->pid;
	}
}

void	reset_stdin_stdout(t_command *command)
{
	if (command->in != -1)
	{
		dup2(g_data.std_in_out[0], STDIN_FILENO);
		command->in = -1;
	}
	if (command->out != -1)
	{
		dup2(g_data.std_in_out[1], STDOUT_FILENO);
		command->out = -1;
	}
}

void	ft_echo(char **argv, size_t count)
{
	int		newline;
	size_t	i;

	newline = 1;
	i = 1;
	if (count && argv[1] && !ft_strcmp(argv[1], "-n"))
	{
		newline = 0;
		++i;
	}
	while (i < count)
	{
		if (argv[i])
		{
			if (count == 2 || (count == 3 && !newline))
				printf("%s", argv[i]);
			else
				printf(" %s", argv[i]);
		}
		++i;
	}
	if (newline)
		printf("\n");
	g_data.exit_status = 0;
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

int	ft_is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

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
	while (*(char *)str && ft_is_digit(*(char *)str))
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
	while (str[i] && !ft_is_digit(str[i]))
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

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

int	ft_isalnum(int c)
{
	if (ft_isdigit(c) || ft_isalpha(c))
		return (1);
	return (0);
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

void	new_exp_el(char *str, char *key, char *value, t_exp_list **alst)
{
	t_exp_list	*new;
	char		*full_str;

	full_str = ft_strdup(str);
	if (!full_str)
		ft_quit(1, 0, "malloc error");
	new = exp_list_new(full_str, key, value);
	add_exp_to_list(new, alst);
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

void	free_exp_el(t_exp_list *el)
{
	free(el->key);
	if (el->value)
		free(el->value);
	free(el->full_str);
	free(el);
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

void	ft_pwd(void)
{
	char	buf[4096];

	g_data.exit_status = 0;
	if (!getcwd(buf, 4095))
	{
		ft_error(NULL, "pwd");
		g_data.exit_status = 1;
	}
	else
		printf("%s\n", buf);
}

void	update_oldpwd(t_exp_list *oldpwd_el,
t_exp_list *pwd_el, char *buf, char *good_val)
{
	char	*fullstr;
	char	*value;

	if (oldpwd_el)
	{
		if (!good_val)
		{
			if (pwd_el && pwd_el->value)
				update_oldpwd(oldpwd_el, pwd_el, buf, pwd_el->value);
			else if (buf)
				update_oldpwd(oldpwd_el, pwd_el, buf, buf);
			return ;
		}
		else
		{
			fullstr = ft_strjoin("OLDPWD=", good_val);
			value = ft_strdup(good_val);
			if (!value || !value)
			{
				free(fullstr);
				ft_quit(1, NULL, "malloc error");
			}
			update_exp_el(fullstr, value, oldpwd_el, 0);
		}
	}
}

void	update_pwd(t_exp_list *pwd_el, char *buf)
{
	char	*fullstr;
	char	*value;

	if (pwd_el)
	{
		if (!buf)
		{
			ft_error(NULL, "updategetcwd");
			g_data.exit_status = 1;
			return ;
		}
		fullstr = ft_strjoin("PWD=", buf);
		value = ft_strdup(buf);
		if (!value || !value)
		{
			free(fullstr);
			ft_quit(1, NULL, "malloc error");
		}
		update_exp_el(fullstr, value, pwd_el, 0);
	}
}

void	chdir_check(char *path, t_exp_list *pwd_el, t_exp_list	*oldpwd_el)
{
	char	buf[4096];
	char	*ptr;
	int		error;

	error = 0;
	if (!getcwd(buf, 4095))
		error = 1;
	if (chdir(path) == -1)
	{
		ft_error(NULL, "cd");
		g_data.exit_status = 1;
		return ;
	}
	if (error)
		ptr = NULL;
	else
		ptr = buf;
	update_oldpwd(oldpwd_el, pwd_el, ptr, NULL);
	update_pwd(pwd_el, ptr);
	update_my_env(&g_data.exp_list);
}

void	handle_cd_expansions(char *path, t_exp_list	*home_el,
t_exp_list *pwd_el, t_exp_list	*oldpwd_el)
{
	char	*newpath;

	if (path[0] == '-')
	{
		if (!oldpwd_el || !oldpwd_el->value)
		{
			builtin_error("cd", NULL, "OLDPWD not set\n");
			g_data.exit_status = 1;
			return ;
		}
		newpath = oldpwd_el->value;
	}
	else
	{
		if (!home_el || !home_el->value)
		{
			builtin_error("cd", NULL, "HOME not set\n");
			g_data.exit_status = 1;
			return ;
		}
		newpath = home_el->value;
	}
	chdir_check(newpath, pwd_el, oldpwd_el);
}

void	ft_cd(char **argv, size_t count, t_exp_list **alst)
{
	t_exp_list	*home_el;
	t_exp_list	*pwd_el;
	t_exp_list	*oldpwd_el;

	home_el = search_el(*alst, "HOME");
	pwd_el = search_el(*alst, "PWD");
	oldpwd_el = search_el(*alst, "OLDPWD");
	if (count == 1 && (!home_el || !home_el->value))
	{
		builtin_error("cd", NULL, "HOME not set\n");
		g_data.exit_status = 1;
		return ;
	}
	else if (count == 1 && home_el && home_el->value)
		chdir_check(home_el->value, pwd_el, oldpwd_el);
	else if (count > 1)
	{
		if (!ft_strcmp("~", argv[1]) || !ft_strcmp("-", argv[1]))
			handle_cd_expansions(argv[1], home_el, pwd_el, oldpwd_el);
		else
			chdir_check(argv[1], pwd_el, oldpwd_el);
	}
}

void	ft_builtin(
	char **argv, size_t count, t_exp_list **alst, t_builtin_t type)
{
	if (type == echo_f)
		ft_echo (argv, count);
	else if (type == exit_f)
		ft_exit (argv, count);
	else if (type == export_f)
		ft_export(argv, count, alst);
	else if (type == env_f)
		ft_env(g_data.myenv);
	else if (type == unset_f)
		ft_unset(argv, count, alst);
	else if (type == cd_f)
		ft_cd(argv, count, alst);
	else if (type == pwd_f)
		ft_pwd();
}

int	verif_pipe(t_line *line)
{
	if (pipe(line->pipe_fd) == -1)
	{
		line->pipe_fd[0] = -1;
		line->pipe_fd[1] = -1;
		ft_error(NULL, "pipe");
		g_data.exit_status = 1;
		return (1);
	}
	else
		return (0);
}

void	exec_builtin(t_line *line, t_command *command)
{
	if (!line->pipeline)
	{
		dup_fd(line, command);
		ft_builtin(command->argv, command->arg_counter,
			&g_data.exp_list, command->builtin);
		reset_stdin_stdout(command);
	}
	else
	{
		if (command->pipe)
			verif_pipe(line);
		command->pid = fork();
		if (!command->pid)
		{
			echo_control_seq(1);
			dup_fd(line, command);
			ft_builtin(command->argv, command->arg_counter,
				&g_data.exp_list, command->builtin);
			exit(g_data.exit_status);
		}
		verif_fork_fail(command->pid);
		close_after_fork(line, command);
		if ((is_last_cmd(line->pipeline, command->pipe, command->builtin)))
			line->last = command->pid;
	}
}

void	ft_exec(t_line *line, t_command *command)
{
	if (command->builtin)
		exec_builtin(line, command);
	else if (!command->exec_name)
		exec_empty_command(line, command, 0);
	else if (!command->builtin && command->exec_name)
		exec_command(line, command);
}

void	manage_command(t_line *line, t_command *command, t_word_list *cursor)
{
	t_word_list	*next_cursor;

	if (cursor)
	{
		next_cursor = delimit_command(line, command, cursor);
		if (perform_redirections(line, command, cursor, next_cursor))
			exec_empty_command(line, command, 1);
		else
		{
			fill_and_expand_args_or_cmd(line, command, cursor, next_cursor);
			ft_exec(line, command);
		}
		clear_command(command);
		manage_command(line, command, next_cursor);
	}
}

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

void	wait_line(t_data *data, t_line *line, t_command *command)
{
	while (1)
	{
		line->str_line = readline("minishell> ");
		if (!line->str_line)
			ft_exit(NULL, 1);
		if (!treat_line(line))
			manage_command(line, command, line->word_list);
		wait_last_process(line->last, &data->exit_status);
		wait_all_process();
		if (line->str_line[0])
			add_history(line->str_line);
		clear_line(line);
		echo_control_seq(0);
	}
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

int	main(int ac, char **av, char **envp)
{
	(void)av;
	if (ac != 1)
	{
		ft_putstr_fd("Error: Minishell does not support argument\n", 2);
		exit (EXIT_FAILURE);
	}
	init(envp);
	wait_line(&g_data, &g_data.line, &g_data.command);
	return (0);
}
