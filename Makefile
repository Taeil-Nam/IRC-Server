NAME		=	ircserv


CXX			=	c++
CXXFLAGS	=	-I./src -MMD #-Wall -Wextra -Werror -std=c++98

SRCS		=	src/main.cpp						\
				$(wildcard src/core/*.cpp)			\
				$(wildcard src/utils/*.cpp)
				
OBJS		=	$(SRCS:.cpp=.o)
DEPS		=	$(SRCS:.cpp=.d)


%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

fclean :
	$(RM) -rf $(OBJS) $(DEPS) $(NAME)
clean :
	$(RM) $(OBJS) $(DEPS)
re :
	$(MAKE) fclean
	$(MAKE) all
	
bonus :

-include $(DEPS)

.PHONY: all clean fclean re


#----------------------------------#
clangd :
	/bin/bash clangd_gen.sh
#----------------------------------#