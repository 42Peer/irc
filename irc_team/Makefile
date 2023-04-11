CXX			=	c++
CXXFLAGS	=	-std=c++98
SRCS		=	main.cpp \
				Server.cpp \
 				Handler.cpp \
				Command.cpp \
				Channel.cpp \

OBJS		=	$(SRCS:.cpp=.o)
NAME		=	ircserv

all : $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
