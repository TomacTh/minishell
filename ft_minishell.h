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

typedef enum e_subtype {r_right, r_left, r_append, here_doc, pipe_t, delimiter, filename, other} t_subtype;

typedef enum e_builtin_t {not, env_f, export_f, unset_f, pwd_f, cd_f, exit_f, echo_f} t_builtin_t;

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
extern t_data		g_data;

void	verif_fork_fail(pid_t pid);
int		verif_pipe(t_line *line);

void	echo_control_seq(int toogle);

void	free_exp_el(t_exp_list *el);
void	clear_exp_list(t_exp_list **alst);
void	clear_line(t_line *line);
void	clear_command(t_command *command);
void	clear_h_doc_tab(t_here_doc **h_doc_tab, size_t *h_doc_len);

int		ft_quit(int error, char *str_error, char *suffix_err);
void	ft_bzero(void *s, size_t n);


int 	break_in_words(char *str, int *i, t_word_list **list);

int		is_ifs(char c);

char	**ft_split(char const *s, char c);
char	**free_split(char **strs, int len);
char	*ft_strjoin(char const *s1, char const *s2);
void	ft_putstr_fd(char *s, int fd);
size_t	ft_strlen(const char *s);
void	close_fd(int *fd);
char	*ft_strjoin_free_s1(char *s1, char *s2);
char	*ft_strjoin_free_s2(char *s1, char *s2);
int		protect_malloc(void **ptr, size_t lenght);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strdup(const char *s);

#endif
