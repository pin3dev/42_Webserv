# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/15 17:50:15 by pin3dev           #+#    #+#              #
#    Updated: 2024/05/27 21:42:02 by pin3dev          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

RED=\033[0;31m
GREEN=\033[0;32m
YELLOW=\033[0;33m
BLUE=\033[0;34m
RESET=\033[0m 
GRAY=\033[37;90m
MAGENTA=\033[35m
BKGRAY=\033[47;90m
CYAN=\033[36m"


# Diretórios
SRC_DIR = ../exe
INC_DIR = ../inc
TEST_DIR = .

# Compilador e flags
CPP = c++
CPPFLAGS = -I$(INC_DIR) -std=c++98 -Wall -Wextra -Werror -pedantic -g

all : help

# Limpeza dos binários
clean:
	rm -rf t_*

# Limpeza completa dos binários e arquivos objeto
fclean: clean
	rm -rf *.o *.dSYM

# Limpeza e compilação
re: fclean menu

# Ajuda
help:
	@echo "							$(BKGRAY)Modo de Uso$(RESET) ⚠️"
	@echo "* $(GREEN)  make [opção]$(RESET)"
	@echo "							$(BKGRAY)Opções $(RESET)⬇️"
	@echo "* $(MAGENTA)  make $(YELLOW)menu $(GRAY) -> exibe diretórios de teste disponiveis para navegação rápida$(RESET)"
	@echo "* $(MAGENTA)  make $(YELLOW)t_<ClassName> $(GRAY) -> compila para teste a ClassName em ./<ClassName>TEST/<ClassName>lTEST.cpp e ../src/<ClassName>.cpp$(RESET)"
	@echo "* $(MAGENTA)  make $(YELLOW)clean$(GRAY) -> remove todos os binários$(RESET)"
	@echo "* $(MAGENTA)  make $(YELLOW)fclean $(GRAY) -> remove todos os binários e arquivos objeto$(RESET)"
	@echo "* $(MAGENTA)  make $(YELLOW)re $(GRAY) -> remove todos os binários e arquivos objeto e compila novamente$(RESET)"

# Menu para selecionar o teste a compilar
menu:
	@echo "Escolhar o $(RED)index$(RESET) da $(BLUE)Classe$(RESET) que deseja testar:"
	@i=1 ; \
	for dir in $$(ls -d *TEST/); do \
		class_name=$${dir%TEST*}; \
		echo "*   $(RED)$$i $(GRAY) - Para Compilar teste de $(BLUE)$$class_name$(RESET)"; \
		i=$$((i + 1)); \
	done; \
	read -p "Digite o número do teste: " index; \
	i=1; \
	for dir in $$(ls -d *TEST/); do \
		if [ "$$i" -eq "$$index" ]; then \
			class_name=$${dir%TEST*}; \
			make t_$$class_name; \
		fi; \
		i=$$((i + 1)); \
	done

# Alvo para compilar testes unitários
t_%:
	$(CPP) $(CPPFLAGS) -o $@ $(TEST_DIR)/$*TEST/$*TEST.cpp $(SRC_DIR)/$*.cpp $(SRC_DIR)/Utils.cpp


