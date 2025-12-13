CC ?= gcc
CXX = g++
CXXFLAGS = -Wall -std=c++17

INC := -I homm3tools/h3m/h3mlib -I homm3tools/3rdparty/uthash/src
LIBDIR := homm3tools/OUTPUT/gcc
STATICLIBS := $(LIBDIR)/h3mlib.a $(LIBDIR)/h3mtilespritegen.a

LUAC ?= $(shell pkg-config --cflags lua)
LUAL ?= $(shell pkg-config --libs   lua)

CFLAGS := $(LUAC) -W -Wall -Wextra -O3 -fPIC -shared -std=c99 $(INC)
LDLIBS := $(LUAL)
LFLAGS := -L $(LIBDIR)

# Library flags
LDFLAGS = -L/usr/lib -llua

HOMM3_SRC := $(shell find homm3lua -type f -name '*.c')
HOMM3_OBJ := $(addprefix dist/,$(notdir $(HOMM3_SRC:.c=.o)))

# Directories
GENERATOR_DIR = ./generator
SRC_DIR = $(GENERATOR_DIR)/.
UTILS_DIR = $(GENERATOR_DIR)/luaUtils
MAP_INFO_DIR = $(GENERATOR_DIR)/mapInfo
GLOBAL_DIR = $(GENERATOR_DIR)/global
LAYOUT_INFO_DIR = $(GENERATOR_DIR)/layoutInfo
MAP_DIR = $(GENERATOR_DIR)/map
PLACERS_DIR = $(MAP_DIR)/placers

# Source files for Generator
GEN_SOURCES = $(SRC_DIR)/Generator.cpp \
              $(LAYOUT_INFO_DIR)/LayoutInfo.cpp \
              $(LAYOUT_INFO_DIR)/ZoneInfo.cpp \
              $(LAYOUT_INFO_DIR)/RegionInfo.cpp \
              $(LAYOUT_INFO_DIR)/ConnectionInfo.cpp \
              $(GLOBAL_DIR)/Random.cpp \
			  $(MAP_DIR)/Map.cpp \
			  $(PLACERS_DIR)/RegionPlacer.cpp \
              $(UTILS_DIR)/luaHelpers.cpp \
			  $(MAP_INFO_DIR)/Tile.cpp \
			  $(MAP_INFO_DIR)/Zone.cpp \
			  $(MAP_INFO_DIR)/Region.cpp

# Object files for Generator
GEN_OBJECTS = $(GEN_SOURCES:.cpp=.o)

# Create necessary directories for object files
OBJECT_DIRS = $(sort $(dir $(GEN_OBJECTS))) dist

# Main targets
all: dist/homm3lua.so Generator

# Create directories
$(OBJECT_DIRS):
	mkdir -p $@

# Generator target
Generator: $(GEN_OBJECTS)
	$(CXX) $(GEN_OBJECTS) -o $@ $(LDFLAGS)

# HOMM3 libraries
.PHONY: libs
libs:
	$(MAKE) -C homm3tools/h3m/h3mtilespritegen/BUILD/gcc install
	$(MAKE) -C homm3tools/h3m/h3mlib/BUILD/gcc install

# HOMM3 Lua target
dist/homm3lua.so: $(HOMM3_OBJ) | libs
	$(CC) $^ $(STATICLIBS) $(CFLAGS) $(LDLIBS) -o $@

# Compilation rules
%.o: %.cpp | $(OBJECT_DIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

dist/%.o: homm3lua/%.c homm3lua/%.h | dist
	$(CC) $< -c $(CFLAGS) -o $@

# Documentation
doc: doc/index.html
doc/index.html: $(HOMM3_SRC)
	ldoc -c /dev/null homm3lua

# Clean
.PHONY: clean
clean:
	$(MAKE) -C homm3tools/h3m/h3mtilespritegen/BUILD/gcc clean
	$(MAKE) -C homm3tools/h3m/h3mlib/BUILD/gcc clean
	rm -rf dist doc $(GEN_OBJECTS) Generator

# Build and run Generator
.PHONY: run
run: Generator
	./Generator

FMT_SRCS := $(shell find . \
    -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | \
    grep -v '^./homm3tools/')

.PHONY: format format-check
format:
	clang-format -i $(FMT_SRCS)

format-check:
	clang-format --dry-run --Werror $(FMT_SRCS)

.PHONY: lint

lint:
	cppcheck \
		--enable=all --inconclusive \
		--std=c++17 --language=c++ \
		--language=c++ \
		--force \
		--suppress=*:lua.h* \
		--suppress=missingIncludeSystem \
		-i homm3lua \
		-i homm3tools \
		.

# Phony targets
.PHONY: all doc clean run