# Exported/Public
ALIAS							= [WEBSERV]
NAME        					= webserv

# Compiler flags
CXX         					= c++
WWW_FLAGS    					= -Wall -Wextra -Werror -g3
DEPS_FLAGS   					= -MMD -MP
CPP98_FLAGS  					= -std=c++98
CXX_FLAGS     					= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

# Tools
RM 								= rm -rf
MKDIR 							= mkdir -p
BUILD_DIR   					= ./.build
MAKE_FLAGS						= --no-print-directory

# Conf files
CONF_DIR   						= ./config
CONF_MK_INCLUDES 				= $(CONF_DIR)/includes.mk
CONF_MK_SOURCES 				= $(CONF_DIR)/sources.mk
CONF_MK_MODULES 				= $(CONF_DIR)/modules.mk

# includes
INCLUDES_DIR					= ./includes
INCLUDES_FLAGS 					:= -I $(INCLUDES_DIR)
-include $(CONF_MK_INCLUDES)

# sources
SRC_DIR							= ./srcs
SRC_FILES						:=
-include $(CONF_MK_SOURCES)

SRCS         					= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS 							= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS 	  						= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRCS))

# libs
MODULES_DIR						= ./modules

MODULE_BNF_DIR 					= $(MODULES_DIR)/bnf
MODULE_BNF_NAME					= $(MODULE_BNF_DIR)/libbnf.a
MODULE_BNF_INCLUDES				= $(MODULE_BNF_DIR)/includes
MODULE_BNF_SRCS					= $(MODULE_BNF_DIR)/config/sources.mk
MODULE_BNF_FLAGS 				= -L $(MODULE_BNF_DIR) -lbnf
-include $(MODULE_BNF_SRCS) 	# SRCS from MODULE_BNF => $(BNF_SRCS)
BNF_SRCS 						:= $(addprefix $(MODULE_BNF_DIR)/, $(BNF_SRCS))

MODULE_LOG_DIR 					= $(MODULES_DIR)/log
MODULE_LOG_NAME					= $(MODULE_LOG_DIR)/liblog.a
MODULE_LOG_INCLUDES				= $(MODULE_LOG_DIR)/includes
MODULE_LOG_SRCS					= $(MODULE_LOG_DIR)/config/sources.mk
MODULE_LOG_FLAGS 				= -L $(MODULE_LOG_DIR) -llog
-include $(MODULE_LOG_SRCS) 	# SRCS from MODULE_LOG => $(LOG_SRCS)
LOG_SRCS 						:= $(addprefix $(MODULE_LOG_DIR)/, $(LOG_SRCS))

MODULE_KIT_DIR 					= $(MODULES_DIR)/kit
MODULE_KIT_NAME					= $(MODULE_KIT_DIR)/libkit.a
MODULE_KIT_INCLUDES				= $(MODULE_KIT_DIR)/includes
MODULE_KIT_SRCS					= $(MODULE_KIT_DIR)/config/sources.mk
MODULE_KIT_FLAGS 				= -L $(MODULE_KIT_DIR) -lkit
-include $(MODULE_KIT_SRCS) 	# SRCS from MODULE_KIT => $(KIT_SRCS)
KIT_SRCS 						:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_SRCS))

MODULES_INCLUDES_FLAGS 			+= -I $(MODULE_BNF_INCLUDES) -I $(MODULE_LOG_INCLUDES) -I $(MODULE_KIT_INCLUDES)
MODULES_FLAGS 					= $(MODULE_BNF_FLAGS) $(MODULE_LOG_FLAGS) $(MODULE_KIT_FLAGS)

INCLUDES_FLAGS					+= $(MODULES_INCLUDES_FLAGS)

# Colors
RED     						= \033[0;31m
GREEN   						= \033[0;32m
YELLOW  						= \033[0;33m
BLUE   							= \033[1;34m
NC      						= \033[0m

# rules
all: $(NAME)

$(NAME): $(OBJS) $(MODULE_BNF_NAME) $(MODULE_LOG_NAME) $(MODULE_KIT_NAME)
	@echo "$(BLUE)$(ALIAS) Linking $(NAME)...$(NC)"
	@$(CXX) $(CXX_FLAGS) $(OBJS) -o $(NAME) $(INCLUDES_FLAGS) $(MODULES_FLAGS) 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@echo "$(GREEN)$(ALIAS) Compiling $<...$(NC)"
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(MODULE_BNF_NAME): $(BNF_SRCS)
	@echo "$(YELLOW)$(ALIAS) Generating bnf module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_BNF_DIR);

$(MODULE_LOG_NAME): $(LOG_SRCS)
	@echo "$(YELLOW)$(ALIAS) Generating log module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_LOG_DIR);

$(MODULE_KIT_NAME): $(KIT_SRCS)
	@echo "$(YELLOW)$(ALIAS) Generating kit module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_KIT_DIR);

clean:
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)
	@$(RM) $(BUILD_DIR)
	@echo "$(RED)$(ALIAS) Cleaned build objects.$(NC)"

fclean: clean
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_KIT_DIR)
	@$(RM) $(NAME)
	@echo "$(RED)$(ALIAS) Removed library and obj.mk.$(NC)"

re: fclean all

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -q -s ./$(NAME) 

run: all
	./$(NAME)
	
.PHONY: all clean fclean re val run

-include $(DEPS)







gen:
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_LOG_DIR);
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_BNF_DIR);
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_KIT_DIR);
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) SRC_FILES 0
	@echo "Generating includes.mk..."
	@./config/include_flags.sh $(SRC_DIR) "*.hpp" $(CONF_MK_INCLUDES) INCLUDES_FLAGS 1
	@echo "Regenerating Makefile..."
	$(MAKE) re

.PHONY: gen
