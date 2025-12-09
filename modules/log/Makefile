# Exported/Public
ALIAS							= [LOG]
NAME        					= liblog.a
EXPORT_OBJS_VAR					= LOG_OBJS
EXPORT_SRCS_VAR					= LOG_SRCS

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

# Includes
INCLUDES_DIR					= ./includes
INCLUDES_FLAGS 					= -I $(INCLUDES_DIR)

# Sources
SRC_DIR							= ./srcs
$(EXPORT_SRCS_VAR)				:=
-include $(CONF_MK_SOURCES)

OBJS 							= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $($(EXPORT_SRCS_VAR)))
DEPS 	  						= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $($(EXPORT_SRCS_VAR)))

# Libs
MODULES_DIR 					= ..
MODULE_KIT_DIR 					= $(MODULES_DIR)/kit
MODULE_KIT_INCLUDES				= $(MODULE_KIT_DIR)/includes
MODULE_KIT_MK_OBJ				= $(MODULE_KIT_DIR)/config/objs.mk
MODULE_KIT_MK_SRCS				= $(MODULE_KIT_DIR)/config/sources.mk
-include $(MODULE_KIT_MK_SRCS)	# SRCS from MODULE_KIT => $(KIT_SRCS)
-include $(MODULE_KIT_MK_OBJ) 	# OBJS from MODULE_KIT => $(KIT_OBJS)
KIT_SRCS 						:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_SRCS))
KIT_OBJS 						:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_OBJS))

MODULE_INCLUDE_FLAGS			= -I $(MODULE_KIT_INCLUDES)
INCLUDES_FLAGS 					+= $(MODULE_INCLUDE_FLAGS)

# Colors
RED     						= \033[0;31m
GREEN   						= \033[0;32m
YELLOW  						= \033[0;33m
BLUE   							= \033[1;34m
NC      						= \033[0m

# Rules
all:  $(NAME)

$(NAME): $(OBJS) $(KIT_OBJS)
	@echo "$(BLUE)$(ALIAS)Archiving $(NAME)...$(NC)"
	@$(AR) $(NAME) $(OBJS) $(KIT_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@echo "$(GREEN)$(ALIAS) Compiling $<...$(NC)"
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(MODULE_KIT_MK_OBJ): $(KIT_SRCS)
	@$(MAKE) $(MAKE_FLAGS) obj -C $(MODULE_KIT_DIR)

$(KIT_OBJS): $(MODULE_KIT_MK_OBJ)
	@$(MAKE) $(MAKE_FLAGS) all -C $(MODULE_KIT_DIR)

-include $(DEPS)

clean:
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)$(ALIAS) Cleaned build objects.$(NC)"

fclean: clean
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_KIT_DIR)
	@$(RM) $(NAME) $(CONF_MK_OBJ)
	@echo "$(RED)$(ALIAS) Removed library and obj.mk.$(NC)"

re: fclean all

obj:
	@echo -n "$(EXPORT_OBJS_VAR)=" > $(CONF_MK_OBJ)
	@for o in $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$($(EXPORT_SRCS_VAR))); do \
		echo -n "$$o " >> $(CONF_MK_OBJ); \
	done
	@echo "" >> $(CONF_MK_OBJ)

.PHONY: all clean fclean re obj gen

gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) $(EXPORT_SRCS_VAR) 1
	@echo "Regenerating Makefile..."
