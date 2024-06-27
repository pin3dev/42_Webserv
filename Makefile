
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
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

%.o: %.cpp
		${CC} ${CPPFLAGS} ${INCLUDE} -c $< -o $@

$(NAME): ${OBJS}
		${CC} ${CPPFLAGS} ${INCLUDE} ${OBJS} -o $@

all:	${NAME}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME}

re: fclean all

.PHONY: all clean re

