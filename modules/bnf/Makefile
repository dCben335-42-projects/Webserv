# Exported/Public
ALIAS							= [BNF]
NAME        					= libbnf.a
EXPORT_OBJS_VAR					= BNF_OBJS
EXPORT_SRCS_VAR					= BNF_SRCS

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

# libs
MODULES_DIR						= ..

MODULE_LOG_DIR 					= $(MODULES_DIR)/log
MODULE_LOG_INCLUDES				= $(MODULE_LOG_DIR)/includes
MODULE_LOG_MK_SRCS				= $(MODULE_LOG_DIR)/config/sources.mk
MODULE_LOG_MK_OBJ				= $(MODULE_LOG_DIR)/config/objs.mk
-include $(MODULE_LOG_MK_SRCS)	# SRCS from MODULE_LOG => $(LOG_SRCS)
-include $(MODULE_LOG_MK_OBJ)	# OBJS from MODULE_LOG => $(LOG_OBJS)
LOG_SRCS 						:= $(addprefix $(MODULE_LOG_DIR)/, $(LOG_SRCS))
LOG_OBJS 						:= $(addprefix $(MODULE_LOG_DIR)/, $(LOG_OBJS))

MODULE_KIT_DIR 					= $(MODULES_DIR)/kit
MODULE_KIT_INCLUDES				= $(MODULE_KIT_DIR)/includes
MODULE_KIT_MK_SRCS				= $(MODULE_KIT_DIR)/config/sources.mk
MODULE_KIT_MK_OBJ				= $(MODULE_KIT_DIR)/config/objs.mk		
-include $(MODULE_KIT_MK_SRCS)	# SRCS from MODULE_KIT => $(KIT_SRCS)
-include $(MODULE_KIT_MK_OBJ) 	# OBJS from MODULE_KIT => $(KIT_OBJS)
KIT_SRCS 						:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_SRCS))
KIT_OBJS 						:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_OBJS))

INCLUDES_FLAGS 					+= -I $(MODULE_LOG_INCLUDES) -I $(MODULE_KIT_INCLUDES)


# Colors
RED     						= \033[0;31m
GREEN   						= \033[0;32m
YELLOW  						= \033[0;33m
BLUE   							= \033[1;34m
NC      						= \033[0m

# rules
all: $(NAME)

$(NAME): $(OBJS) $(LOG_OBJS) $(KIT_OBJS)
	@echo "$(BLUE)$(ALIAS) Archiving $(NAME)...$(NC)"
	@$(AR) $(NAME) $(OBJS) $(LOG_OBJS) $(KIT_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@echo "$(GREEN)$(ALIAS) Compiling $<...$(NC)"
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(MODULE_LOG_MK_OBJ): $(LOG_SRCS)
	@$(MAKE) $(MAKE_FLAGS) obj -C $(MODULE_LOG_DIR)

$(LOG_OBJS): $(MODULE_LOG_MK_OBJ)
	@$(MAKE) $(MAKE_FLAGS) all -C $(MODULE_LOG_DIR)

$(MODULE_KIT_MK_OBJ): $(KIT_SRCS)
	@$(MAKE) $(MAKE_FLAGS) obj -C $(MODULE_KIT_DIR)

$(KIT_OBJS): $(MODULE_KIT_MK_OBJ)
	@$(MAKE) $(MAKE_FLAGS) all -C $(MODULE_KIT_DIR)

-include $(DEPS)

clean:
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)$(ALIAS) Cleaned build objects.$(NC)"

fclean: clean
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_LOG_DIR)
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


.PHONY: all clean fclean re obj 

gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) $(EXPORT_SRCS_VAR) 1
	@echo "Regenerating Makefile..."

.PHONY: gen