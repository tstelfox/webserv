# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tmullan <tmullan@student.codam.nl>           +#+                      #
#                                                    +#+                       #
#    Created: 2022/02/01 12:43:01 by tmullan       #+#    #+#                  #
#    Updated: 2022/02/04 18:37:28 by tmullan       ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = server
CXX = clang++
SRC = server_main.cpp sockets/*.cpp
OBJ_DIR = obj
SRC_DIR = src
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Isrc/sockets



all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ $(FLAGS) $(INCLUDES) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all