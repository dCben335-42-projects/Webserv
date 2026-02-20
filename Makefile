# Exported/Public
ALIAS							= [WEBSERV]
NAME        					= webserv

# Compiler flags
CXX         					= c++
WWW_FLAGS    					= -Wall -Wextra -Werror
EVERYTHING_FLAGS 				= 
DEPS_FLAGS   					= -MMD -MP
CPP98_FLAGS  					= -std=c++98
CXX_FLAGS     					= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS) $(EVERYTHING_FLAGS)

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
MODULE_BNF_FLAGS 				= -L $(MODULE_BNF_DIR) -lbnf

MODULE_LOG_DIR 					= $(MODULES_DIR)/log
MODULE_LOG_NAME					= $(MODULE_LOG_DIR)/liblog.a
MODULE_LOG_INCLUDES				= $(MODULE_LOG_DIR)/includes
MODULE_LOG_FLAGS 				= -L $(MODULE_LOG_DIR) -llog

MODULE_KIT_DIR 					= $(MODULES_DIR)/kit
MODULE_KIT_NAME					= $(MODULE_KIT_DIR)/libkit.a
MODULE_KIT_INCLUDES				= $(MODULE_KIT_DIR)/includes
MODULE_KIT_FLAGS 				= -L $(MODULE_KIT_DIR) -lkit

MODULE_JSON_DIR 				= $(MODULES_DIR)/json
MODULE_JSON_NAME				= $(MODULE_JSON_DIR)/libjson.a
MODULE_JSON_INCLUDES			= $(MODULE_JSON_DIR)/includes
MODULE_JSON_FLAGS 				= -L $(MODULE_JSON_DIR) -ljson

MODULES_INCLUDES_FLAGS 			+= -I $(MODULE_BNF_INCLUDES) -I $(MODULE_LOG_INCLUDES) -I $(MODULE_KIT_INCLUDES) -I $(MODULE_JSON_INCLUDES)
MODULES_FLAGS 					= $(MODULE_BNF_FLAGS) $(MODULE_LOG_FLAGS) $(MODULE_KIT_FLAGS) $(MODULE_JSON_FLAGS)

INCLUDES_FLAGS					+= $(MODULES_INCLUDES_FLAGS)

# Colors
RED     						= \033[0;31m
GREEN   						= \033[0;32m
YELLOW  						= \033[0;33m
BLUE   							= \033[1;34m
NC      						= \033[0m

TOTAL_SRCS 						:= $(words $(SRCS))
COMPILED_SRCS					:= 0

# rules
all: $(NAME)

$(NAME): $(MODULE_BNF_NAME) $(MODULE_LOG_NAME) $(MODULE_KIT_NAME) $(MODULE_JSON_NAME) $(OBJS)
	@echo "\n$(BLUE)$(ALIAS) Linking $(NAME)...$(NC)"
	@$(CXX) $(CXX_FLAGS) $(OBJS) -o $(NAME) $(INCLUDES_FLAGS) $(MODULES_FLAGS) 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	@$(eval COMPILED_SRCS=$(shell echo $$(($(COMPILED_SRCS)+1))))
	@progress=$$(printf '#%.0s' $$(seq 1 $$(($(COMPILED_SRCS)*50/$(TOTAL_SRCS))))); \
	printf "\r$(GREEN)$(ALIAS) Compiling $(NAME): [%-50s] %d/%d$(NC)" "$$progress" $(COMPILED_SRCS) $(TOTAL_SRCS);
	@$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(MODULE_BNF_NAME): force
	@echo "$(YELLOW)$(ALIAS) Generating bnf module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_BNF_DIR);

$(MODULE_LOG_NAME): force
	@echo "$(YELLOW)$(ALIAS) Generating log module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_LOG_DIR);

$(MODULE_KIT_NAME): force
	@echo "$(YELLOW)$(ALIAS) Generating kit module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_KIT_DIR);

$(MODULE_JSON_NAME): force
	@echo "$(YELLOW)$(ALIAS) Generating json module library...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) -C $(MODULE_JSON_DIR);

clean:
	@echo "$(RED)$(ALIAS) Cleaning build objects...$(NC)"
	@$(RM) $(BUILD_DIR)

clean_libs: clean
	@echo "$(YELLOW)$(ALIAS) Cleaning module libraries...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_JSON_DIR)

fclean_libs: fclean
	@echo "$(YELLOW)$(ALIAS) Removing libraries and obj.mk...$(NC)"
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_KIT_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_JSON_DIR)

fclean: clean
	@echo "$(RED)$(ALIAS) Removing $(NAME)...$(NC)"
	@$(RM) $(NAME)

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -q -s ./$(NAME) 

run: all
	@echo "$(GREEN)$(ALIAS) Running $(NAME)...$(NC)"
	@echo "--------------------------\n"
	@./$(NAME)

re_libs: fclean_libs all
re: fclean all

force:
	@true
	
.PHONY: all clean fclean re val run force fclean_libs clean_libs

-include $(DEPS)







gen:
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_LOG_DIR);
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_BNF_DIR);
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_KIT_DIR);
	$(MAKE) $(MAKE_FLAGS) gen -C $(MODULE_JSON_DIR);
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) SRC_FILES 0
	@echo "Generating includes.mk..."
	@./config/include_flags.sh $(SRC_DIR) "*.hpp" $(CONF_MK_INCLUDES) INCLUDES_FLAGS 1
	@echo "Regenerating Makefile..."
	$(MAKE) re

.PHONY: gen
