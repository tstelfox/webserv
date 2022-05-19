# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tmullan <tmullan@student.codam.nl>           +#+                      #
#                                                    +#+                       #
#    Created: 2022/02/01 12:43:01 by tmullan       #+#    #+#                  #
#    Updated: 2022/05/19 13:04:03 by akramp        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = server
CXX = c++
SRC = server_main.cpp sockets/socket.cpp responseHandler/responseHandler.cpp \
		$(SERVER_PREFIX) exceptions/exceptions.cpp $(PARSE_PREFIX) $(CGI_PREFIX)
SERVER_PREFIX = $(addprefix server/, $(SERV))
PARSE_PREFIX = $(addprefix parsing/, $(PARSE))
CGI_PREFIX = $(addprefix cgi/, $(CGI))
SERV = client.cpp poller.cpp serverConfig.cpp Location.cpp
PARSE = Pars_add_vars_to_vec.cpp Pars_helper_funcs.cpp Pars_setters.cpp Parsing.cpp
CGI = cgi.cpp
OBJ_DIR = obj
SRC_DIR = src
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
FLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Isrc/utils -Isrc/sockets -Isrc/server -Isrc/parsing \
		-Isrc/exceptions -Isrc/responseHandler
ifdef DEBUG
  FLAGS += -g
endif

X           = \033[38;5;
Y           = 22
Z           = m
COLOR_RESET = \033[37m
COLOR_INC   = $(eval Y=$(shell echo $$((($(Y)%15)+1))))
GREY        = \033[0;253m

all: $(NAME)

$(NAME): $(OBJ)
	@echo " ───── ❝ 𝗰𝗼𝗺𝗽𝗶𝗹𝗶𝗻𝗴 𝗪𝗲𝗯𝗦𝗲𝗿𝘃 ❞ ───── "
	@echo "$(GREY)"
	$(CXX) $^ $(FLAGS)  $(INCLUDES) -o $(NAME)
	@echo "$(COLOR_RESET)"
	@echo " ─────────── ❝ 𝗱𝗼𝗻𝗲 ❞ ──────────── \n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(COLOR_INC)
	@echo "$(X)$(Y)$(Z)"
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $@
	@echo "$(COLOR_RESET)"

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
