# ── Output binary name ────────────────────────────────────────────────────────
BINARY = fogt

# ── Directories ───────────────────────────────────────────────────────────────
SRC_DIR     = src
INCLUDE_DIR = include
LIBS_DIR    = libs
BUILD_DIR   = build

# ── Sources ───────────────────────────────────────────────────────────────────
SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# ── Target detection ──────────────────────────────────────────────────────────
# Override with: make TARGET=windows-x64  (or macos-x64, macos-aarch64)
TARGET ?= linux-x64

ifeq ($(TARGET), linux-x64)
  CC     = gcc
  OUT    = $(BINARY)
  EXTRA  = -lm -lpthread -ldl

else ifeq ($(TARGET), windows-x64)
  CC     = x86_64-w64-mingw32-gcc
  OUT    = $(BINARY).exe
  EXTRA  = -lwinmm -lgdi32 -lopengl32 -mwindows

else ifeq ($(TARGET), macos-x64)
  CC     = o64-clang
  OUT    = $(BINARY)-macos-x64
  EXTRA  = -framework IOKit -framework Cocoa -framework OpenGL

else ifeq ($(TARGET), macos-aarch64)
  CC     = aarch64-apple-darwin-clang
  OUT    = $(BINARY)-macos-aarch64
  EXTRA  = -framework IOKit -framework Cocoa -framework OpenGL

else
  $(error Unknown TARGET "$(TARGET)". Valid: linux-x64, windows-x64, macos-x64, macos-aarch64)
endif

# ── Flags ─────────────────────────────────────────────────────────────────────
CFLAGS  = -I$(INCLUDE_DIR) -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS = -L$(LIBS_DIR)/$(TARGET) -lraylib -llua $(EXTRA)

# ── Rules ─────────────────────────────────────────────────────────────────────
.PHONY: all clean

all: $(BUILD_DIR) $(OUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	find $(SRC_DIR) -type d | sed 's|$(SRC_DIR)|$(BUILD_DIR)|' | xargs mkdir -p

# Link
$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile each .c → build/*.o  (mirrors src/ subdirectory structure)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BINARY) $(BINARY).exe $(BINARY)-macos-x64 $(BINARY)-macos-aarch64
