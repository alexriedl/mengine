ROOT_DIR=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
APP_DIR=$(realpath $(ROOT_DIR)/app)
SRC_DIR=$(realpath $(APP_DIR)/src)
MAIN_DIR=$(realpath $(SRC_DIR)/main)
CODE_DIR=$(realpath $(MAIN_DIR)/cpp)
BUILD_DIR=$(ROOT_DIR)build/linux

#OPT_FLAGS="-O3"
OPT_FLAGS=-DDEBUG -g
WARNING_FLAGS=-Wall -Werror -Wno-write-strings -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-result -Wno-strict-aliasing -Wno-switch -fno-rtti -fno-exceptions -Wno-sign-compare -std=gnu++11 -Wno-format-security
COMPILE_FLAGS=-DMENGINE_INTERNAL=1 -DMENGINE_SLOW=1 -DMENGINE_SDL=1
OPEN_GL_FLAGS=-lGL
SDL_FLAGS=-ldl $(shell sdl2-config --cflags --libs) -Wl,-rpath,'$ORIGIN/x86_64'

all: platform game

platform:
	@mkdir -p $(BUILD_DIR)
	@printf "Compiling Platform\n"
	@cd $(BUILD_DIR) && g++ $(OPT_FLAGS) $(WARNING_FLAGS) $(COMPILE_FLAGS) "$(CODE_DIR)/sdl_mengine.cpp" -I$(CODE_DIR) -o mengine.x86_64 $(SDL_FLAGS) $(OPEN_GL_FLAGS)

game:
	@printf "Compiling Game\n"
	@cd $(BUILD_DIR) && c++ $(OPT_FLAGS) $(WARNING_FLAGS) $(COMPILE_FLAGS) "$(CODE_DIR)/mengine.cpp" -fPIC -shared -o mengine.so.temp
	@cd $(BUILD_DIR) && mv mengine.so.temp mengine.so
