CXX			= clang++
NAME		= webserv
SRC			= $(wildcard srcs/*.cpp)
OBJ			= $(SRC:.cpp=.o)
CXXFLAGS	= -I./inc -std=c++98 -Wall -Wextra -g  -Werror -D LOG 

ifdef DEBUG
CXXFLAGS	+= -D DEBUG
endif

all			: $(NAME)

$(NAME)		: $(OBJ) 
			$(CXX) $(CXXFLAGS) -o $@ $^

clean		:
			rm -rf $(SRC:.cpp=.o)

fclean		: clean
			rm -rf $(NAME) test

re			: fclean all

reclean		: fclean all clean

.PHONY : all clean fclean re reclean
