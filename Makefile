NAME = ircserv

SRCS_DIR = ./srcs
HEADER_DIR = ./includes/
OBJS_DIR = ./objs

vpath %.cpp $(SRCS_DIR)

SRCS =	main.cpp/ \
	utils/Db.cpp utils/Command.cpp utils/Parsing.cpp \
	server/Server.cpp server/Handler.cpp \

OBJ = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
OBJS = $(addprefix $(OBJS_DIR)/, $(OBJ))
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD
RM = rm -f
RMDIR = rm -rf

all : $(NAME)
	
$(NAME) : $(OBJS)
	@echo "\033[0;34m====Compiling :\033[0;33m" $@ "\033[0;34m===="
	@$(CC) $(CFLAGS) -I $(HEADER_DIR) $(OBJS) -o $@
	@echo "\033[0;33m" $@ "HAS BEEN CREATED"


$(OBJS_DIR)/%.o : %.cpp
	@mkdir -p $(OBJS_DIR)
	@$(CC) $(CFLAGS) -I $(HEADER_DIR) -c $< -o $@

help :
	@echo "make re"
	@echo "./ircserv <port_number> <password>"


clean :
	@echo "\033[0;31mREMOVE OBJECTIVE FILES"
	@$(RMDIR) $(OBJS_DIR)

fclean : clean
	@echo "\033[0;31mREMOVE $(NAME)"
	@$(RM) $(NAME)

re : fclean all

.PHONY : all help clean fclean re

-include $(DEPS)
