NAME		=	woody_woodpacker

UNAME		=	$(shell uname)

ASM			=	nasm
ASMFLAGS	=	-f

CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror -Ofast -I $(INCLUDE) -I libft/includes/ -g #-fsanitize=address

SRCDIR		=	./src
C_SRCDIR	=	$(SRCDIR)/c/
ASM_SRCDIR	=	$(SRCDIR)/asm/
OBJDIR		=	./obj/
INCLUDE		=	./include/
PAYLOADDIR	=	./payloads/
LIBFT		=	./libft/libft.a

C_SRC		=	main.c	\
				utils.c

ifeq ($(UNAME), Linux)
	C_SRC		+=	elf64.c	\
					elf32.c

	CRYPT		=	rc4_elf64.s
	PAYLOAD_SRC	=	handler_elf64.s		\
					parasite_elf64.s	\
					rc4_elf64.s			\
					handler_elf32.s		\
					parasite_elf32.s	\
					rc4_elf32.s

	PAYLOADS	=	${addprefix $(PAYLOADDIR), $(PAYLOAD_SRC:%.s=%.bin)}

else
	C_SRC		+=	macho.c
	PAYLOAD_SRC	=	parasite.c
	PAYLOADS	=	${addprefix $(PAYLOADDIR), $(PAYLOAD_SRC:%.c=%.dynl)}
endif

OBJ			=	${addprefix $(OBJDIR), $(C_SRC:%.c=%.o)}

ifeq ($(UNAME), Linux)
	OBJ		+=	${addprefix $(OBJDIR), $(CRYPT:%.s=%.o)}
endif


# ===== #


all:					$(NAME)

$(NAME):				$(LIBFT) $(OBJDIR) $(OBJ) $(PAYLOADDIR) $(PAYLOADS)
						$(CXX) $(CXXFLAGS) $(OBJ) -Llibft -lft -o $(NAME)

clean:
						rm -rf $(NAME) woody

fclean:					clean
						rm -rf $(OBJDIR) $(PAYLOADDIR)
						make -C libft/ fclean

re:						fclean all

$(LIBFT):
						make -C libft/

$(OBJDIR):
						@mkdir -p $(OBJDIR)

$(OBJDIR)%.o:			$(C_SRCDIR)%.c
						$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:			$(ASM_SRCDIR)%.s
						$(ASM) $(ASMFLAGS) elf64 $< -o $@

$(PAYLOADDIR):
						@mkdir -p $(PAYLOADDIR)

$(PAYLOADDIR)%.bin:		$(ASM_SRCDIR)%.s
						$(ASM) $(ASMFLAGS) bin $< -o $@

$(PAYLOADDIR)%.dynl:	$(C_SRCDIR)%.c
						$(CXX) $(CXXFLAGS) -dynamiclib $< -o $@

.PHONY:					re clean fclean obj all
