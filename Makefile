NAME        	= webserv


CXX         			= c++
WWW_FLAGS    			= -Wall -Wextra -Werror -g3
DEPS_FLAGS   			= -MMD -MP
CPP98_FLAGS  			= -std=c++98
CXX_FLAGS     			= $(WWW_FLAGS) $(CPP98_FLAGS) $(DEPS_FLAGS)

RM 						= rm -rf
MKDIR 					= mkdir -p
BUILD_DIR   			= ./.build
MAKE_FLAGS				= --no-print-directory

CONF_DIR   				= ./config
CONF_MK_INCLUDES 		= $(CONF_DIR)/includes.mk
CONF_MK_SOURCES 		= $(CONF_DIR)/sources.mk
CONF_MK_MODULES 		= $(CONF_DIR)/modules.mk

# includes
INCLUDES_DIR			= ./includes
INCLUDES_FLAGS 			:= -I $(INCLUDES_DIR)
-include $(CONF_MK_INCLUDES)

# sources
SRC_DIR					= ./srcs
SRC_FILES				:=
-include $(CONF_MK_SOURCES)

# libs
MODULES_DIR				= ./modules

MODULE_BNF_DIR 			= $(MODULES_DIR)/bnf
MODULE_BNF_NAME			= $(MODULE_BNF_DIR)/libbnf.a
MODULE_BNF_INCLUDES		= $(MODULE_BNF_DIR)/includes
MODULE_BNF_FLAGS 		= -L $(MODULE_BNF_DIR) -lbnf

MODULE_LOG_DIR 			= $(MODULES_DIR)/log
MODULE_LOG_NAME			= $(MODULE_LOG_DIR)/liblog.a
MODULE_LOG_INCLUDES		= $(MODULE_LOG_DIR)/includes
MODULE_LOG_FLAGS 		= -L $(MODULE_LOG_DIR) -llog

MODULE_KIT_DIR 			= $(MODULES_DIR)/kit
MODULE_KIT_NAME			= $(MODULE_KIT_DIR)/libkit.a
MODULE_KIT_INCLUDES		= $(MODULE_KIT_DIR)/includes
MODULE_KIT_FLAGS 		= -L $(MODULE_KIT_DIR) -lkit

MODULES_INCLUDES_FLAGS 	+= -I $(MODULE_BNF_INCLUDES) -I $(MODULE_LOG_INCLUDES) -I $(MODULE_KIT_INCLUDES)
MODULES_FLAGS 			= $(MODULE_BNF_FLAGS) $(MODULE_LOG_FLAGS) $(MODULE_KIT_FLAGS)

INCLUDES_FLAGS			+= $(MODULES_INCLUDES_FLAGS)

SRCS         			= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS 					= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS 	  				= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRCS))

# rules
$(NAME): $(OBJS) $(MODULE_BNF_NAME) $(MODULE_LOG_NAME) $(MODULE_KIT_NAME)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(NAME) $(INCLUDES_FLAGS) $(MODULES_FLAGS) 

all: $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(MODULE_BNF_NAME): force
	$(MAKE) $(MAKE_FLAGS) -C $(MODULE_BNF_DIR);

$(MODULE_LOG_NAME): force
	$(MAKE) $(MAKE_FLAGS) -C $(MODULE_LOG_DIR);

$(MODULE_KIT_NAME): force
	$(MAKE) $(MAKE_FLAGS) -C $(MODULE_KIT_DIR);

clean:
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) clean -C $(MODULE_KIT_DIR)
	$(RM) $(BUILD_DIR)

fclean: clean
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_BNF_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_LOG_DIR)
	@$(MAKE) $(MAKE_FLAGS) fclean -C $(MODULE_KIT_DIR)
	$(RM) $(NAME)

re: fclean all

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -q -s ./$(NAME) 

run: all
	./$(NAME)

force:
	@true

	
.PHONY: all clean fclean re val run force

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
