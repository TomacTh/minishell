#ifndef FT_MINISHELL_H
# define FT_MINISHELL_H
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <limits.h>
# include <stdio.h>
# include <signal.h>
# include <fcntl.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <errno.h>
# include <string.h>

typedef enum e_type {word, op} t_type;
typedef enum e_subtype {r_right,
						r_left,
						r_append,
						here_doc,
						pipe_t,
						delimiter,
						filename,
						 other} t_subtype;

typedef enum e_builtin_t {not,
						env_f,
						export_f,
						unset_f,
						pwd_f,
						cd_f,
						exit_f,
						echo_f} t_builtin_t;

typedef struct s_exp_list
{
	char				*full_str;
	char				*key;
	char				*value;
	struct s_exp_list	*next;
}					t_exp_list;

typedef struct s_word_list
{
	t_type				type;
	t_subtype			subtype;
	size_t				begin;
	size_t				len;
	struct s_word_list	*next;
}					t_word_list;

typedef struct s_here_doc
{
	size_t		index;
	int			fd;
	char		*name;
	char		*delimiter;
	t_word_list	*word;
}				t_here_doc;

typedef struct s_line
{
	char		*str_line;
	int			pipeline;
	int			pipe_fd[2];
	pid_t		last;
	t_word_list	*word_list;
	size_t		h_doc_i;
	size_t		h_doc_len;
	t_here_doc	*h_doc_tab;
}				t_line;


typedef struct s_command
{
	t_builtin_t	builtin;
	pid_t		pid;
	int			pipe;
	int			in;
	int			out;
	char		*exec_name;
	char		*path;
	char		*argv[4096];
	size_t		arg_counter;

}				t_command;

typedef struct s_data
{
	t_command	command;
	t_line		line;
	char		**split_path;
	char		**myenv;
	t_exp_list	*exp_list;
	int			std_in_out[2];
	int			exit_status;
	int			nonewline;
}				t_data;

typedef struct	s_parse_f
{
	char	*filename;
	size_t	i;
	size_t	end;
	size_t	len;
	size_t	locklen;
	size_t	lock_empty;
	char	quote;
	int		init;
}				t_parse_f;

typedef struct	s_parse
{
	char	buffer[4096];
	size_t	i;
	size_t	end;
	size_t	len;
	char	quote;
}				t_parse;

/*
** This globale variable is create to count all characters printing
*/
extern t_data	g_data;


void	ctrlc_handler(int num);
void	hdoc_handler(int num);

void	reset_stdin_stdout(t_command *command);
int		is_last_cmd(int pipeline, int pipe, t_builtin_t builtin);
int		contain_ifs(char *str);

void	init(char **envp);

void	create_update_split_path(char ***split_path, char *path);
char	*get_command_path(t_command *command);


void	update_my_env(t_exp_list **alst);

void	echo_control_seq(int toogle);
void	verif_fork_fail(pid_t pid);
int		verif_pipe(t_line *line);

void	close_fd(int *fd);
void	dup_fd(t_line *line, t_command *command);
void	switch_fd(int newvalue, int *fd);
int		open_h_doc_fd(t_line *line, t_command *command, int *newfd);
int		open_filename_fd(t_command *command, char *buffer, t_subtype redir);

int		ft_quit(int error, char *str_error, char *suffix_err);
int		ambigous_redirection(char *str, size_t len);
void	ft_error(char *error, char *suffix_err);
void	builtin_error(char *program_name, char *arg, char *error);
void	free_exp_el(t_exp_list *el);
void	clear_exp_list(t_exp_list **alst);
void	clear_line(t_line *line);
void	clear_command(t_command *command);
void	clear_h_doc_tab(t_here_doc **h_doc_tab, size_t *h_doc_len);

void	ft_exec(t_line *line, t_command *command);
void	exec_empty_command(t_line *line, t_command *command, int code);
void	exec_path_no_find(t_line *line, t_command *command);
void	get_env_var_len(t_exp_list **alst, size_t	*i);

char	*ft_udecimaltoa(size_t n);

t_word_list	*get_del_word(t_word_list *el, int index);
char		*get_delimiter_str(t_word_list *delimiter, char *line);
int			treat_heredocs(t_line *line, t_word_list **alst, size_t *here_doc_len);
void		init_here_docs(t_here_doc *h_doc_tab, size_t *here_doc_len);
int			fill_heredocs(t_here_doc *h_doc_tab, size_t h_doc_len, t_line *line);

int			key_is_valid(char *key);
void		key_is_not_valid(char *program_name, char *arg, char *key, char *value);


void	ft_builtin(
	char **argv, size_t count, t_exp_list **alst, t_builtin_t type);
void	ft_echo(char **argv, size_t count);
void	ft_exit(char **argv, size_t count);
int		ft_env(char **envp);
void	ft_pwd(void);
void	ft_cd(char **argv, size_t count, t_exp_list **alst);
void	ft_export(char **argv, size_t count, t_exp_list **alst);
void	ft_unset(char **argv, size_t count, t_exp_list **alst);

t_word_list	*new_word_list(t_type type, int i);



void	fill_expand_space_filename(t_parse_f *p_data, char *str);
int		expansion_redir(char *str_line, t_parse_f *p_data);
void	manage_dollar_redir(char *str_line, t_parse_f *p_data);
int		perform_redirections(
	t_line *line, t_command *command, t_word_list *cursor,
	t_word_list	*n_cursor);

void		new_exp_el(char *str, char *key, char *value, t_exp_list **alst);
t_exp_list	*search_el(t_exp_list *list, char *key);
void		update_exp_el(char *str, char *value, t_exp_list *el, int dup);
void		add_exp_to_list(t_exp_list *new, t_exp_list **list);
t_exp_list	*exp_list_new(char *str, char *key, char *value);

int			is_op(int c);
int			is_ifs(int c);
int			ft_isalpha(int c);
int			ft_isalnum(int c);
int			ft_isdigit(int c);
int			contain_ifs(char *str);
int			no_expand(char *str_line);


void	fil_str(char *str, size_t num, char *base);

void	set_exitstatus_fromerrno(void);
void	verif_fork_fail(pid_t pid);
int		verif_pipe(t_line *line);
void	wait_all_process(void);
void	wait_last_process(pid_t last, int *exit_code);
void	close_after_fork(t_line *line, t_command *command);

int		tag_words_and_ops(t_word_list **alst);
int 	break_in_words(char *str, int *i, t_word_list **list);



void	get_key(char *key, char *str_line, size_t *i, size_t *end);
void	get_key_and_value(char *str, char **key, char **value);
int		get_expand_quote(char *buff, char *value, size_t *len);
int		get_expand_exitcode(char *buffer, size_t *len);

void	new_execname(char *str, t_command *command);

void	init_parse_data(t_parse *parse_data, size_t end, size_t begin);
void	init_parse_f_data(t_parse_f *parse_data, size_t end,
size_t begin, char buffer[4096]);




char		*search_str(t_exp_list *list, char *key);
void		get_exec_and_args(char *str_line, t_word_list *el, t_command *cmd);

void		add_char(char *str, char *line, size_t *len, size_t *i);
int			is_quoted(char c);
char		quote_value(char c);
void		update_quote(char *quote, char *str_line, size_t *i);
void		end_quote(char *quote, size_t *i);

t_word_list	*new_word_list(t_type type, int i);

void		ft_bzero(void *s, size_t n);
void		*ft_memcpy(void *dst, const void *src, size_t n);
char		**ft_split(char const *s, char c);
char		**free_split(char **strs, int len);
int			protect_malloc(void **ptr, size_t lenght);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
int			ft_strnchr(char c, char *str, size_t n);
char		*ft_strdup(const char *s);
char		*ft_strndup(const char *s, size_t size);
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_strjoin_free_s1(char *s1, char *s2);
char		*ft_strjoin_free_s2(char *s1, char *s2);
void		ft_putstr_fd(char *s, int fd);
size_t		ft_strlen(const char *s);

#endif
