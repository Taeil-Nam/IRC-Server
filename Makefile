NAME		=	ircserv

CXX			=	c++
CXXFLAGS	=	-I./src	-I./extlibs/libbsd-gdf/include -MMD -Wall -Wextra -Werror -std=c++98

LDFLAGS		=	-L./extlibs/libbsd-gdf/lib -Wl,-rpath,./extlibs/libbsd-gdf/lib
LDLIBS		=	-lbsd-gdf-assert -lbsd-gdf-logger -lbsd-gdf-display -lbsd-gdf-network -lbsd-gdf-event

SRCS		=	src/main.cpp						\
				$(wildcard src/grc/core/*.cpp)		\
				$(wildcard src/utils/*.cpp)
				
OBJS		=	$(SRCS:.cpp=.o)
DEPS		=	$(SRCS:.cpp=.d)


%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(MAKE) all -C extlibs/libbsd-gdf
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

fclean :
	$(MAKE) fclean -C extlibs/libbsd-gdf
	$(RM) -rf $(OBJS) $(DEPS) $(NAME)
clean :
	$(MAKE) clean -C extlibs/libbsd-gdf
	$(RM) $(OBJS) $(DEPS)
re :
	$(MAKE) fclean
	$(MAKE) all
	
bonus :

-include $(DEPS)

.PHONY: all clean fclean re


#----------------------------------#
clangd :
	echo "CompileFlags:" > .clangd
	echo "  Add: [-I$(PWD)/src, -I$(PWD)/extlibs/libbsd-gdf/include, -std=c++98]" >> .clangd


lib :
	git submodule init
	git submodule update

lib-update:
	git submodule update --remote
#----------------------------------#