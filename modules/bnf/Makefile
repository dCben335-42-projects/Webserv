NAME        			= libbnf.a
EXPORT_OBJS_VAR			= BNF_OBJS

AR 						= ar -rcs
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
CONF_MK_SOURCES 		= $(CONF_DIR)/sources.mk
CONF_MK_OBJ 			= $(CONF_DIR)/obj.mk

# includes
INCLUDES_DIR			= ./includes
INCLUDES_FLAGS 			= -I $(INCLUDES_DIR)

# sources
SRC_DIR					= ./srcs
SRC_FILES				:=
-include $(CONF_MK_SOURCES)

# libs
MODULES_DIR				= ..

MODULE_LOG_DIR 			= $(MODULES_DIR)/log
MODULE_LOG_INCLUDES		= $(MODULE_LOG_DIR)/includes
MODULE_LOG_MK_OBJ		= $(MODULE_LOG_DIR)/config/obj.mk
# OBJS from MODULE_LOG => $(LOG_OBJS)
-include $(MODULE_LOG_MK_OBJ) 
LOG_OBJS 				:= $(addprefix $(MODULE_LOG_DIR)/, $(LOG_OBJS))

MODULE_KIT_DIR 			= $(MODULES_DIR)/kit
MODULE_KIT_INCLUDES		= $(MODULE_KIT_DIR)/includes
MODULE_KIT_MK_OBJ		= $(MODULE_KIT_DIR)/config/obj.mk		
# OBJS from MODULE_KIT => $(KIT_OBJS)
-include $(MODULE_KIT_MK_OBJ)
KIT_OBJS 				:= $(addprefix $(MODULE_KIT_DIR)/, $(KIT_OBJS))

INCLUDES_FLAGS 			+= -I $(MODULE_LOG_INCLUDES) -I $(MODULE_KIT_INCLUDES)

SRCS         			= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS 					= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS 	  				= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRCS))

# rules
all: $(NAME)

$(NAME): $(OBJS) $(LOG_OBJS) $(KIT_OBJS) 
	$(AR) $(NAME) $(OBJS) $(LOG_OBJS) $(KIT_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile $(MODULE_LOG_MK_OBJ) $(MODULE_KIT_MK_OBJ)
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES_FLAGS)

$(CONF_MK_OBJ): $(OBJS)
	@echo -n "$(EXPORT_OBJS_VAR)=" > $@
	@for o in $(OBJS); do echo -n "$$o " >> $@; done
	@echo "" >> $@

$(MODULE_LOG_MK_OBJ): force
	@$(MAKE) $(MAKE_FLAGS) obj -C $(MODULE_LOG_DIR)

$(MODULE_KIT_MK_OBJ): force
	@$(MAKE) $(MAKE_FLAGS) obj -C $(MODULE_KIT_DIR)


-include $(DEPS)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME) $(CONF_MK_OBJ)

re: fclean all

obj: $(CONF_MK_OBJ)

force:
	@true

.PHONY: all clean fclean re force 





gen:
	@$(MKDIR) $(CONF_DIR)
	@echo "Generating sources.mk..."
	@./config/generate_mk.sh $(SRC_DIR) "*.cpp" $(CONF_MK_SOURCES) SRC_FILES 0
	@echo "Regenerating Makefile..."

.PHONY: gen