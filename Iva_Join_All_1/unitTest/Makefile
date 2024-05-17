# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/15 17:50:15 by pin3dev           #+#    #+#              #
#    Updated: 2024/05/17 18:52:46 by pin3dev          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Diretórios
SRC_DIR = ../exe
INC_DIR = ../inc
TEST_DIR = .

# Compilador e flags
CPP = c++
CPPFLAGS = -I$(INC_DIR) -std=c++98 -Wall -Wextra -Werror -pedantic -g

# Alvo padrão
all: help

# Alvo para compilar testes unitários
t_%: $(TEST_DIR)/%TEST.cpp $(SRC_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

# Limpeza dos binários
clean:
	rm -rf t_*

# Limpeza completa dos binários e arquivos objeto
fclean: clean
	rm -rf *.o *.dSYM

# Ajuda
help:
	@echo "Uso:"
	@echo "  make t_<ClassName> - compila o arquivo <ClassName>TEST.cpp no diretório atual"
	@echo "Exemplo:"
	@echo "  make t_Request - compila RequestTEST.cpp e ../src/Request.cpp"

