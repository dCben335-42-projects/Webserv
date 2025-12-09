# Exported/Public
ALIAS							= [KIT]
NAME        					= libkit.a
EXPORT_OBJS_VAR					= KIT_OBJS
EXPORT_SRCS_VAR					= KIT_SRCS


# Compiler flags
AR 								= ar -rcs
CXX         					= c++
WWW_FLAGS    					= -Wall -Wextra -Werror -g3
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

# rules
all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)$(ALIAS)Archiving $(NAME)...$(NC)"
	@$(AR) $(NAME) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@echo "$(GREEN)$(ALIAS) Compiling $<...$(NC)"
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

clean:
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)$(ALIAS) Cleaned build objects.$(NC)"

fclean: clean
	@$(RM) $(NAME) $(CONF_MK_OBJ)
	@echo "$(RED)$(ALIAS) Removed library and obj.mk.$(NC)"

re: fclean all

obj:
	@echo -n "$(EXPORT_OBJS_VAR)=" > $(CONF_MK_OBJ)
	@for o in $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$($(EXPORT_SRCS_VAR))); do \
		echo -n "$$o " >> $(CONF_MK_OBJ); \
	done
	@echo "" >> $(CONF_MK_OBJ)

.PHONY: all clean fclean re obj

-include $(DEPS)


gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) $(EXPORT_SRCS_VAR) 1
	@echo "Regenerating Makefile..."

.PHONY: gen