# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: robin <robin@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 16:42:52 by mguerga           #+#    #+#              #
#    Updated: 2024/05/22 12:10:09 by mguerga          ###   ########.fr        #
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
		src/srvrblk_n_srvrloc_logic.cpp \
		src/upload.cpp \
		src/cgi_handler.cpp \
		src/request_handler.cpp \
		src/Err_page_switch.cpp \
		src/request_parser_utils.cpp

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
	mkdir -p upload
	make -C cgi_bin/

clean:
	rm -f $(OBJS) $(OBJS2)
	@$(RM)r upload
	make clean -C cgi_bin/

fclean: clean
	rm -f $(NAME)
	make fclean -C cgi_bin/

re: fclean all

.PHONY: clean fclean re
