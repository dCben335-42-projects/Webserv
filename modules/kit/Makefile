# Exported/Public
ALIAS							= [KIT]
NAME        					= libkit.a
EXPORT_OBJS_VAR					= KIT_OBJS
EXPORT_SRCS_VAR					= KIT_SRCS


# Compiler flags
AR 								= ar -rcs
CXX         					= c++
WWW_FLAGS    					= -Wall -Wextra -Werror
DEPS_FLAGS   					= -MMD -MP
CPP98_FLAGS  					= -std=c++98
CXX_FLAGS     					= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

# Tools
RM 								= rm -rf
MKDIR 							= mkdir -p
MAKE_FLAGS						= --no-print-directory
BUILD_DIR   					= ./.build

# Conf files
CONF_DIR   						= ./config
CONF_MK_SOURCES 				= $(CONF_DIR)/sources.mk
CONF_MK_OBJ 					= $(CONF_DIR)/objs.mk

# includes
INCLUDES_DIR					= ./includes
INCLUDES_FLAGS 					= -I $(INCLUDES_DIR)

# sources
SRC_DIR							= ./srcs
$(EXPORT_SRCS_VAR)				:=
-include $(CONF_MK_SOURCES)

OBJS 							= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $($(EXPORT_SRCS_VAR)))
DEPS 	  						= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $($(EXPORT_SRCS_VAR)))

# Colors
RED     						= \033[0;31m
GREEN   						= \033[0;32m
YELLOW  						= \033[0;33m
BLUE   							= \033[1;34m
NC      						= \033[0m

TOTAL_SRCS 						:= $(words $($(EXPORT_SRCS_VAR)))
COMPILED_SRCS					:= 0

# rules
all: $(NAME)

$(NAME): $(OBJS)
	@echo "\n$(BLUE)$(ALIAS) Archiving $(NAME)...$(NC)"
	@$(AR) $(NAME) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@$(eval COMPILED_SRCS=$(shell echo $$(($(COMPILED_SRCS)+1))))
	@progress=$$(printf '#%.0s' $$(seq 1 $$(($(COMPILED_SRCS)*50/$(TOTAL_SRCS))))); \
	printf "\r$(GREEN)$(ALIAS) Compiling $(NAME): [%-50s] %d/%d$(NC)" "$$progress" $(COMPILED_SRCS) $(TOTAL_SRCS);
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

clean:
	@echo "$(RED)$(ALIAS) Cleaning build objects...$(NC)"
	@$(RM) $(BUILD_DIR)

fclean: clean
	@echo "$(RED)$(ALIAS) Removed library and obj.mk.$(NC)"
	@$(RM) $(NAME) $(CONF_MK_OBJ)

re: fclean all

obj:
	@echo -n "$(EXPORT_OBJS_VAR)=" > $(CONF_MK_OBJ)
	@for o in $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$($(EXPORT_SRCS_VAR))); do \
		echo -n "$$o " >> $(CONF_MK_OBJ); \
	done
	@echo "" >> $(CONF_MK_OBJ)

.PHONY: all clean fclean re obj

-include $(DEPS)

