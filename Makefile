NAME        	= webserv


CXX         	= c++
WWW_FLAGS    	= -Wall -Wextra -Werror
DEPS_FLAGS   	= -MMD -MP
CPP98_FLAGS  	= -std=c++98
CXX_FLAGS     	= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

RM 				= rm -rf
MKDIR 			= mkdir -p
BUILD_DIR   	= ./.build

CONF_DIR   = ./config
CONF_MK_INCLUDES = $(CONF_DIR)/includes.mk
CONF_MK_SOURCES = $(CONF_DIR)/sources.mk

# includes
INCLUDES_FLAGS :=
-include $(CONF_MK_INCLUDES)

# sources
SRC_DIR		= ./srcs
SRC_FILES	:=
-include $(CONF_MK_SOURCES)

SRC         = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ 		= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# rules
$(NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $(NAME) $(INCLUDES_FLAGS)

all: $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -q -s ./$(NAME) 

run: all
	./$(NAME)

gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) SRC_FILES
	@echo "Generating includes.mk..."
	@./config/include_flags.sh . "*.hpp" $(CONF_MK_INCLUDES) INCLUDES_FLAGS
	@echo "Regenerating Makefile..."
	$(MAKE)

.PHONY: all clean fclean re val run gen