NAME = ircserv

SRCS_DIR = ./srcs
HEADER_DIR = ./includes/
OBJS_DIR = ./objs

vpath %.cpp $(SRCS_DIR)

SRC =	main.cpp \
	utils/Db.cpp utils/Command.cpp utils/Parsing.cpp \
	server/Server.cpp server/Handler.cpp \

SRCS = $(addprefix $(SRCS_DIR)/, $(SRC))
OBJ = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)
OBJS = $(addprefix $(OBJS_DIR)/, $(OBJ))
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD
RM = rm -f
RMDIR = rm -rf

all : $(NAME)
	
$(NAME) : $(OBJS)
	@echo "\033[0;34m====Compiling :\033[0;33m" $@ "\033[0;34m===="
	@$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) $(OBJS) -o $@
	@echo "\033[0;33m" $@ "HAS BEEN CREATED"


$(OBJS_DIR)/%.o : $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -I $(HEADER_DIR) -c $< -o $@

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
