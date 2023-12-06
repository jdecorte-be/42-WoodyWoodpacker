NAME		=	libft.a
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror -I $(INCLUDE)
SRCDIR		=	./src/
OBJDIR		=	./obj/
INCLUDE		=	./include/
AR			=	ar -rc

SRC			=	ft_atoi.c		\
				ft_bzero.c		\
				ft_calloc.c		\
				ft_isalnum.c	\
				ft_isalpha.c	\
				ft_isascii.c	\
				ft_isdigit.c	\
				ft_isprint.c	\
				ft_itoa.c		\
				ft_memchr.c		\
				ft_memcmp.c		\
				ft_memmove.c	\
				ft_memset.c		\
				ft_putchar_fd.c	\
				ft_putendl_fd.c	\
				ft_putnbr_fd.c	\
				ft_putstr_fd.c	\
				ft_split.c		\
				ft_strchr.c		\
				ft_strdup.c		\
				ft_striteri.c	\
				ft_strjoin.c	\
				ft_strlcat.c	\
				ft_strlcpy.c	\
				ft_strlen.c		\
				ft_strmapi.c	\
				ft_strncmp.c	\
				ft_strnstr.c	\
				ft_strrchr.c	\
				ft_strtrim.c	\
				ft_swap.c		\
				ft_substr.c		\
				ft_tolower.c	\
				ft_toupper.c	\
				
OBJ			=	${addprefix $(OBJDIR), $(SRC:%.c=%.o)}


# ===== #


all:			$(NAME)

$(NAME):		$(OBJDIR) $(OBJ)
				$(AR) $(NAME) $(OBJ)

clean:
				rm -rf $(OBJDIR)

fclean:			
				rm -rf $(NAME) $(OBJDIR)

re:				fclean all

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean all
