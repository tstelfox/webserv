# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tmullan <tmullan@student.codam.nl>           +#+                      #
#                                                    +#+                       #
#    Created: 2022/02/01 12:43:01 by tmullan       #+#    #+#                  #
#    Updated: 2022/02/04 18:13:36 by tmullan       ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = server
CXX = clang++
SRC = server_main.cpp socket.cpp
OBJ_DIR = obj
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++98
# INCLUDES = -Isrc/vector -Isrc/map -Isrc/stack -Isrc/iterators -Isrc/utils



all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ $(FLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all