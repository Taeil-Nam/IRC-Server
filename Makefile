NAME		=	ircserv


CXX			=	c++
CXXFLAGS	=	-I./src #-Wall -Wextra -Werror -std=c++98

SRCS		=	src/main.cpp						\
				$(wildcard src/core/*.cpp)			\
				$(wildcard src/utils/*.cpp)
				
OBJS		=	$(SRCS:.cpp=.o)


%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

fclean :
	$(RM) -rf $(OBJS) $(NAME)
clean :
	$(RM) $(OBJS)
re :
	$(MAKE) fclean
	$(MAKE) all
	
bonus :


.PHONY: all clean fclean re


#----------------------------------#
clangd :
	/bin/bash clangd_gen.sh
#----------------------------------#