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
TEMPLATE_INFO_DIR = $(GENERATOR_DIR)/templateInfo
MAP_DIR = $(GENERATOR_DIR)/map
PLACERS_DIR = $(MAP_DIR)/placers

COMMON_SOURCES = \
			  $(TEMPLATE_INFO_DIR)/TemplateInfo.cpp \
			  $(TEMPLATE_INFO_DIR)/ZoneTemplate.cpp \
			  $(TEMPLATE_INFO_DIR)/ConnectionTemplate.cpp \
              $(GLOBAL_DIR)/Random.cpp \
              $(GLOBAL_DIR)/CellularAutomata.cpp \
			  $(PLACERS_DIR)/ZonePlacer.cpp \
			  $(PLACERS_DIR)/ObjectPlacer.cpp \
			  $(PLACERS_DIR)/TownPlacer.cpp \
			  $(PLACERS_DIR)/ConnectionPlacer.cpp \
			  $(PLACERS_DIR)/BorderPlacer.cpp \
			  $(PLACERS_DIR)/GuardPlacer.cpp \
			  $(PLACERS_DIR)/GuardHandler.cpp \
			  $(PLACERS_DIR)/TerrainPlacer.cpp \
              $(UTILS_DIR)/luaHelpers.cpp \
			  $(MAP_DIR)/Map.cpp \
			  $(MAP_INFO_DIR)/Tile.cpp \
			  $(MAP_INFO_DIR)/Object.cpp \
			  $(MAP_INFO_DIR)/Town.cpp \
			  $(MAP_INFO_DIR)/Zone.cpp \
			  $(MAP_INFO_DIR)/Obstacle.cpp \
			  $(MAP_INFO_DIR)/Creature.cpp \
			  $(MAP_INFO_DIR)/Treasure.cpp \
			  $(MAP_INFO_DIR)/Resource.cpp \
			  $(MAP_INFO_DIR)/Artifact.cpp \
			  $(MAP_INFO_DIR)/PandoraBox.cpp \
			  $(MAP_INFO_DIR)/Mine.cpp \
			  $(MAP_INFO_DIR)/Road.cpp

# Source files for Generator
GEN_SOURCES = $(SRC_DIR)/Generator.cpp $(COMMON_SOURCES)

# Source files for Units generator
UNITS_SOURCES = ./tests/Units.cpp $(COMMON_SOURCES)

# Object files for Generator
## Place object files for sources under generator/ into `build/`
OBJ_DIR := build

# Object files for Generator (map generator/*.cpp -> build/*.o)
GEN_OBJECTS = $(GEN_SOURCES:.cpp=.o)
GEN_OBJECTS := $(patsubst $(GENERATOR_DIR)/%.o,$(OBJ_DIR)/%.o,$(GEN_OBJECTS))

# Object files for Units (keep test objects local, but move generator parts)
UNITS_OBJECTS = $(UNITS_SOURCES:.cpp=.o)
UNITS_OBJECTS := $(patsubst $(GENERATOR_DIR)/%.o,$(OBJ_DIR)/%.o,$(UNITS_OBJECTS))

# Create necessary directories for object files
OBJECT_DIRS = $(sort $(dir $(GEN_OBJECTS) $(UNITS_OBJECTS))) dist

# Main targets
all: dist/homm3lua.so Generator Units

# Create directories
$(OBJECT_DIRS):
	mkdir -p $@

# Generator target
Generator: $(GEN_OBJECTS)
	$(CXX) $(GEN_OBJECTS) -o $@ $(LDFLAGS)

# Units target
Units: $(UNITS_OBJECTS)
	$(CXX) $(UNITS_OBJECTS) -o $@ $(LDFLAGS)

# HOMM3 libraries
.PHONY: libs
libs:
	$(MAKE) -C homm3tools/h3m/h3mtilespritegen/BUILD/gcc install
	$(MAKE) -C homm3tools/h3m/h3mlib/BUILD/gcc install

# HOMM3 Lua target
dist/homm3lua.so: $(HOMM3_OBJ) | libs
	$(CC) $^ $(STATICLIBS) $(CFLAGS) $(LDLIBS) -o $@

# Compilation rules
# Rule to compile generator sources into build/<path>.o
$(OBJ_DIR)/%.o: $(GENERATOR_DIR)/%.cpp | $(OBJECT_DIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Fallback rule for other sources (tests etc.)
%.o: %.cpp | $(OBJECT_DIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

dist/%.o: homm3lua/%.c homm3lua/%.h | dist
	$(CC) $< -c $(CFLAGS) -o $@

# Documentation
doc: doc/index.html
doc/index.html: $(HOMM3_SRC)
	ldoc -c /dev/null homm3lua

# Hard Clean
.PHONY: hard_clean
hard_clean:
	$(MAKE) -C homm3tools/h3m/h3mtilespritegen/BUILD/gcc clean
	$(MAKE) -C homm3tools/h3m/h3mlib/BUILD/gcc clean
	rm -rf dist doc $(OBJ_DIR) Generator Units

# (Soft) Clean
.PHONY: clean
clean:
	rm -rf dist doc $(OBJ_DIR) Generator Units

# Build and run Generator
.PHONY: run
run: Generator
	./Generator

.PHONY: units
units: dist/homm3lua.so Units
	./Units

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
.PHONY: all doc clean run units hard_clean
