CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror #-m32
ASM			=	nasm -felf64
ASMDIR		=	../../src/asm/
OBJDIR		=	./obj/

ASM_SRC		=	rc4_elf64.s
C_SRC		=	test.c

OBJ			=	${addprefix $(OBJDIR), $(C_SRC:%.c=%.o)}
OBJ			+=	${addprefix $(OBJDIR), $(ASM_SRC:%.s=%.o)}


# ===== #


all:			$(OBJDIR) $(OBJ)
				$(CXX) $(CXXFLAGS) $(OBJ)

clean:
				rm -rf a.out

fclean:			clean
				rm -rf $(OBJDIR)

re:				fclean all

$(OBJDIR)%.o:	%.c
				$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:	$(ASMDIR)%.s
				$(ASM) $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean obj all
