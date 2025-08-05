
RED=\033[0;31m
GREEN=\033[0;32m
RESET=\033[0m
BKGRAY=\033[47;90m
BKGRAYGREEN=\033[1;92m
BKGRAYRED=\033[1;91m

################################################################################
# 									Executable	 	 						   #
################################################################################

NAME = webserv

################################################################################
# 							Dir	Path	Variables 							   #
################################################################################

SRCDIR = exe
OBJDIR = obj
INCDIR = inc

################################################################################
# 						Compiler and Flags	Variables						   #
################################################################################

CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -I $(INCDIR)
RM = rm -f
RMD = rm -rf

################################################################################
# 							Files	Path	Variables 						   #
################################################################################

SRC_FILES = main.cpp \
			WebServ.cpp \
			Server.cpp \
			Parser.cpp \
			Configurator.cpp \
			Manager.cpp \
			Connect.cpp \
			Request.cpp \
			Utils.cpp \
			CGI.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRC_FILES))
OBJS = $(addprefix $(OBJDIR)/, $(SRC_FILES:.cpp=.o))

################################################################################
# 							Manual	Targets		 							   #
################################################################################

all: $(NAME)

clean:
	@if [ -d "$(OBJDIR)" ]; then $(RMD) $(OBJDIR); fi
	@echo "${BKGRAY}Webserv:${BKGRAYRED}- Obj${RESET}"

fclean: clean
	@$(RM) $(NAME)
	@echo "${BKGRAY}Webserv: ${BKGRAYRED}- Exec${RESET}"

re: fclean all

################################################################################
# 						Auto Targets 				 						   #
################################################################################

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME) & \
	./gui_load.sh "Webserv"
	@wait

################################################################################
# 										PS		 	 						   #
################################################################################

.PHONY: all clean fclean re

