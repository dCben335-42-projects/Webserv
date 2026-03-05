# Exported/Public
ALIAS							= [BNF]
NAME        					= libbnf.a
EXPORT_OBJS_VAR					= BNF_OBJS
EXPORT_SRCS_VAR					= BNF_SRCS

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

TOTAL_SRCS 						:= $(words $($(EXPORT_SRCS_VAR)))
COMPILED_SRCS					:= 0

# rules
all: $(NAME)

$(NAME): $(LOG_OBJS) $(KIT_OBJS) $(OBJS)
	@echo "\n$(BLUE)$(ALIAS) Archiving $(NAME)...$(NC)"
	@$(AR) $(NAME) $(OBJS) $(LOG_OBJS) $(KIT_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@$(eval COMPILED_SRCS=$(shell echo $$(($(COMPILED_SRCS)+1))))
	@progress=$$(printf '#%.0s' $$(seq 1 $$(($(COMPILED_SRCS)*50/$(TOTAL_SRCS))))); \
	printf "\r$(GREEN)$(ALIAS) Compiling $(NAME): [%-50s] %d/%d$(NC)" "$$progress" $(COMPILED_SRCS) $(TOTAL_SRCS);
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

clean_libs:
	@echo "$(YELLOW)$(ALIAS) Cleaning module libraries...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)

fclean_libs:
	@echo "$(YELLOW)$(ALIAS) Removed libraries and obj.mk.$(NC)"
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_KIT_DIR)

simple_clean: 
	@echo "$(RED)$(ALIAS) Cleaning build objects...$(NC)"
	@$(RM) $(BUILD_DIR)

clean: simple_clean clean_libs

fclean: simple_clean fclean_libs
	@echo "$(RED)$(ALIAS) Removed library and obj.mk.$(NC)"
	@$(RM) $(NAME) $(CONF_MK_OBJ)

re: fclean_libs all

obj:
	@echo -n "$(EXPORT_OBJS_VAR)=" > $(CONF_MK_OBJ)
	@for o in $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$($(EXPORT_SRCS_VAR))); do \
		echo -n "$$o " >> $(CONF_MK_OBJ); \
	done
	@echo "" >> $(CONF_MK_OBJ)


.PHONY: all clean_libs fclean_libs simple_clean clean fclean re obj
