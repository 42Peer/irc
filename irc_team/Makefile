CXX			=	c++
CXXFLAGS	=	-std=c++98 -g -fsanitize=address
SRCS		=	main.cpp \
				Server.cpp \
 				Handler.cpp \
				Command.cpp \
				Channel.cpp \
				Parsing.cpp \

OBJS		=	$(SRCS:.cpp=.o)
NAME		=	ircserv

all : $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
