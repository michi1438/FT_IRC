# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgodtsch <rgodtsch@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/21 10:14:15 by mguerga           #+#    #+#              #
#    Updated: 2024/05/23 15:06:09 by rgodtsch         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME_DIR_LISTING = dir_listing.cgi 

NAME_SIMPLE = erreurSegfault.cgi 

NAME_TEST = loop.cgi 

NAME_ENV = simple.cgi

CXX = c++

CXXFLAGS = -Werror -Wall -Wextra -std=c++98

DIR_LISTING = dir_listing.cpp

SIMPLE = erreurSegfault.cpp

TEST = loop.cpp

ENV = simple.cpp

OBJS_DIR_LISTING = $(DIR_LISTING:cpp=o)

OBJS_SIMPLE= $(SIMPLE:cpp=o)

OBJS_TEST= $(TEST:cpp=o)

OBJS_ENV= $(ENV:cpp=o)

all: $(NAME_DIR_LISTING) 

$(NAME_DIR_LISTING): $(OBJS_DIR_LISTING) $(OBJS_SIMPLE) $(OBJS_TEST) $(OBJS_ENV)
	$(CXX) $(CXXFLAGS) $(OBJS_DIR_LISTING) -o $(NAME_DIR_LISTING)
	$(CXX) $(CXXFLAGS) $(OBJS_TEST) -o $(NAME_TEST)
	$(CXX) $(CXXFLAGS) $(OBJS_SIMPLE) -o $(NAME_SIMPLE)
	$(CXX) $(CXXFLAGS) $(OBJS_ENV) -o $(NAME_ENV)

clean:
	rm -f $(OBJS_DIR_LISTING) $(OBJS_TEST) $(OBJS_SIMPLE) $(OBJS_ENV) 
	rm -f dir_listing.html 

fclean: 
	rm -f $(NAME_DIR_LISTING) $(NAME_TEST) $(NAME_SIMPLE) $(NAME_ENV)

re: fclean all

.PHONY: clean fclean re
