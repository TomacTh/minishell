CC		=	gcc
NAME	=	minishell
CFLAGS	= 	-ggdb -Wall -Wextra -Werror 
SRCS	=	$(addprefix ./srcs/, ft_builtin1.c ft_error.c ft_exp_list.c ft_ftfunc.c ft_handler_and_utils.c ft_init.c \
								ft_parse_and_expand.c ft_redir.c ft_small_stuff.c ft_tag.c ft_word.c ft_cd.c \
								ft_exec.c ft_export.c ft_general.c ft_here_doc1.c ft_is.c ft_parse_utils.c \
								ft_utils_parse.c ft_redir_expand.c ft_split.c ft_unset.c ft_clear.c ft_exit.c ft_fd.c \
								ft_get.c ft_heredoc2.c ft_minishell.c ft_path_functions.c ft_str.c ft_small_stuff2.c)
ifeq ($(LOGNAME), tcharvet)
	LIB		=	-L/Users/tcharvet/.brew/Cellar/readline/8.1.1/lib -lreadline
	INC		=	-I./inc -I/Users/tcharvet/.brew/Cellar/readline/8.1.1/include
else
    LIB		=	lreadline
    INC		=	-I./inc
endif
OBJS	=	$(SRCS:.c=.o)

all:	$(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME):	$(OBJS) $(OBJSUTILS)
	$(CC) $(CFLAGS) $(INC) $(OBJS) -o $(NAME) $(LIB)

clean:
	rm -rf $(OBJS)

fclean:	clean
	rm -f $(NAME) $(NAME_BONUS)

re:	fclean all

.PHONY:	all clean fclean re
