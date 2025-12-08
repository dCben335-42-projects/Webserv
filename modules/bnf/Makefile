NAME        		= libbnf.a

AR 					= ar -rcs
CXX         		= c++
WWW_FLAGS    		= -Wall -Wextra -Werror -g3
DEPS_FLAGS   		= -MMD -MP
CPP98_FLAGS  		= -std=c++98
CXX_FLAGS     		= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

RM 					= rm -rf
MKDIR 				= mkdir -p
BUILD_DIR   		= ./.build

CONF_DIR   			= ./config
CONF_MK_SOURCES 	= $(CONF_DIR)/sources.mk

# includes
INCLUDES_DIR			= ./includes
INCLUDES_FLAGS 			= -I $(INCLUDES_DIR)

# sources
SRC_DIR				= ./srcs
SRC_FILES			:=
-include $(CONF_MK_SOURCES)

SRCS         		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS 				= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS 	  			= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRCS))

# rules
$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

all: $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) SRC_FILES 0
	@echo "Regenerating Makefile..."
	$(MAKE) re

.PHONY: all clean fclean re gen

-include $(DEPS)