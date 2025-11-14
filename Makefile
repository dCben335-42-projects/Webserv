NAME        	= webserv


CXX         		= c++
WWW_FLAGS    		= -Wall -Wextra -Werror -g3
DEPS_FLAGS   		= -MMD -MP
CPP98_FLAGS  		= -std=c++98
CXX_FLAGS     		= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

RM 					= rm -rf
MKDIR 				= mkdir -p
BUILD_DIR   		= ./.build

CONF_DIR   			= ./config
CONF_MK_INCLUDES 	= $(CONF_DIR)/includes.mk
CONF_MK_SOURCES 	= $(CONF_DIR)/sources.mk
CONF_MK_MODULES 	= $(CONF_DIR)/modules.mk

# includes
INCLUDES_FLAGS 		:=
-include $(CONF_MK_INCLUDES)

# sources
SRC_DIR				= ./srcs
SRC_FILES			:=
-include $(CONF_MK_SOURCES)

# modules
MODULES_DIR			= ./modules
MODULES_FILES		:= 
-include $(CONF_MK_MODULES)

SRCS         		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
SRCS_OBJS 			= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
SRCS_DEPS 	  		= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRCS))

MODULES        		= $(addprefix $(MODULES_DIR)/, $(MODULES_FILES))
MODULES_OBJS 		= $(patsubst $(MODULES_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(MODULES))
MODULES_DEPS 	  	= $(patsubst $(MODULES_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(MODULES))

OBJS				= $(SRCS_OBJS) $(MODULES_OBJS)
DEPS				= $(SRCS_DEPS) $(MODULES_DEPS)

# rules
$(NAME): $(OBJS)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(NAME) $(INCLUDES_FLAGS)

all: $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(BUILD_DIR)/%.o: $(MODULES_DIR)/%.cpp Makefile
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
	@./config/generate_mk.sh $(MODULES_DIR) "*.cpp" $(CONF_MK_MODULES) MODULES_FILES
	@echo "Generating includes.mk..."
	@./config/include_flags.sh . "*.hpp" $(CONF_MK_INCLUDES) INCLUDES_FLAGS
	@echo "Regenerating Makefile..."
	$(MAKE) re

.PHONY: all clean fclean re val run gen

-include $(DEPS)