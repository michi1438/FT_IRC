# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgodtsch <rgodtsch@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/21 10:14:15 by mguerga           #+#    #+#              #
#    Updated: 2024/05/23 13:44:48 by rgodtsch         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME_DIR_LISTING = dir_listing.cgi 

NAME_SIMPLE = erreurSegfault.cgi 

NAME_TEST = loop.cgi 

CXX = c++

CXXFLAGS = -Werror -Wall -Wextra -std=c++98

DIR_LISTING = dir_listing.cpp

SIMPLE = erreurSegfault.cpp

TEST = loop.cpp

OBJS_DIR_LISTING = $(DIR_LISTING:cpp=o)

OBJS_SIMPLE= $(SIMPLE:cpp=o)

OBJS_TEST= $(TEST:cpp=o)

all: $(NAME_DIR_LISTING) 

$(NAME_DIR_LISTING): $(OBJS_DIR_LISTING) $(OBJS_SIMPLE) $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) $(OBJS_DIR_LISTING) -o $(NAME_DIR_LISTING)
	$(CXX) $(CXXFLAGS) $(OBJS_TEST) -o $(NAME_TEST)
	$(CXX) $(CXXFLAGS) $(OBJS_SIMPLE) -o $(NAME_SIMPLE)

clean:
	rm -f $(OBJS_DIR_LISTING) $(OBJS_TEST) $(OBJS_SIMPLE)
	rm -f dir_listing.html 

fclean: 
	rm -f $(NAME_DIR_LISTING) $(NAME_TEST) $(NAME_SIMPLE)

re: fclean all

.PHONY: clean fclean re
