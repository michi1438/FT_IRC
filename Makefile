# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: robin <robin@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 16:42:52 by mguerga           #+#    #+#              #
#    Updated: 2024/04/17 09:04:26 by lzito            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

OS := $(shell uname)

NAME = webserv

CXX = c++

CXXFLAGS = -Werror -Wall -Wextra -std=c++98

SRCS = 	src/main.cpp \
		src/ConfigFile.cpp \
		src/RequestParser.cpp \
		src/ParsingException.cpp \
		src/upload/upload.cpp

ifeq ($(OS), Darwin)
SRCS2 = src/webserv_kqueue.cpp
endif

ifeq ($(OS), Linux)
SRCS2 = src/webserv_epoll.cpp
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
