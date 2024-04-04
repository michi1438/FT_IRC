# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 16:42:52 by mguerga           #+#    #+#              #
#    Updated: 2024/04/04 14:44:30 by lzito            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

OS := $(shell uname)

NAME = webserv

CXX = c++

CXXFLAGS = -Werror -Wall -Wextra -std=c++98

SRCS = main.cpp ConfigFile.cpp ParsingException.cpp

ifeq ($(OS), Darwin)
SRCS2 = webserv_kqueue.cpp
endif

ifeq ($(OS), Linux)
SRCS2 = webserv_epoll.cpp
endif

OBJS = $(SRCS:cpp=o)

OBJS2 = $(SRCS2:cpp=o)

all: $(NAME)

$(NAME): $(OBJS) $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OBJS) $(OBJS2) -o $(NAME)

clean:
	rm -f $(OBJS) $(OBJS2)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re
