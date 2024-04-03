# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 16:42:52 by mguerga           #+#    #+#              #
#    Updated: 2024/04/03 12:47:43 by mguerga          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CXX = g++

CXXFLAGS = -Werror -Wall -Wextra -std=c++98

SRCS = main.cpp ConfigFile.cpp ParsingException.cpp webserv.cpp


OBJS = $(SRCS:cpp=o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) -lkqueue

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re

