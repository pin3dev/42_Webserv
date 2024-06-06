# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/15 17:50:15 by pin3dev           #+#    #+#              #
#    Updated: 2024/06/06 15:48:13 by pin3dev          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

SRC = exe/main.cpp \
		exe/WebServ.cpp \
		exe/Server.cpp \
		exe/Parser.cpp \
		exe/Configurator.cpp \
		exe/Manager.cpp \
		exe/Connect.cpp \
		exe/Request.cpp \
		exe/Utils.cpp \
		exe/CGI.cpp \

OBJS = ${SRC:.cpp=.o}

INCLUDE = -I inc

CC = c++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -pedantic -std=c++98 -g #-fsanitize=address

.cpp.o:
		${CC} ${CPPFLAGS} ${INCLUDE} -c $< -o ${<:.cpp=.o}

$(NAME): ${OBJS}
		${CC} ${CPPFLAGS} ${INCLUDE} ${OBJS} -o ${NAME}

all:	${NAME}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME}

re: clean all

.PHONY: all clean re