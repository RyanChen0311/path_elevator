# ===========================================================================
# PathElevator (ud) — Makefile
# ===========================================================================
#
# Targets:
#   make          Build release binary (./ud)
#   make debug    Build with AddressSanitizer + debug symbols
#   make clean    Remove build artefacts
#
# ===========================================================================

CXX     = g++

# On Windows (MinGW/MSYS2) the linker appends .exe automatically;
# make clean must remove both to stay tidy.
ifeq ($(OS),Windows_NT)
    TARGET := ud.exe
else
    TARGET := ud
endif

SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

CXXFLAGS_COMMON  = -Wall -Wextra -Wpedantic -I$(INC_DIR) -std=c++17
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O2
CXXFLAGS_DEBUG   = $(CXXFLAGS_COMMON) -g3 -O0 -fsanitize=address,undefined

CXXFLAGS ?= $(CXXFLAGS_RELEASE)

.PHONY: all debug clean

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo ""
	@echo "  Build complete -> ./$(TARGET)"
	@echo "  Run with: ./$(TARGET)"
	@echo ""

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: clean all

clean:
	rm -rf $(BUILD_DIR) ud ud.exe
