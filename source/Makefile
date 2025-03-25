CC = gcc
CFLAGS = -Wall -Wextra -g -I.
LDFLAGS = $(shell sdl2-config --libs)

# Optimization flags for release build
RELEASE_CFLAGS = -O3 -march=native -flto -DNDEBUG

SDL_CONFIG  ?= pkg-config sdl2
CFLAGS_SDL  ?= $(shell $(SDL_CONFIG) --cflags)
LDFLAGS_SDL ?= $(shell $(SDL_CONFIG) --libs)
CFLAGS += $(CFLAGS_SDL)

# Build directory
BUILD_DIR = build
RELEASE_BUILD_DIR = build/release

# Source files
SRCS = Main.c
SRCS += Burger.c
SRCS += Data.c
SRCS += Doors.c
SRCS += EnMove.c
SRCS += EnThink.c
SRCS += InterMis.c
SRCS += Intro.c
SRCS += Level.c
SRCS += Lynx.c
SRCS += Missiles.c
SRCS += Music.c
SRCS += PlMove.c
SRCS += PlStuff.c
SRCS += PlThink.c
SRCS += PushWall.c
SRCS += RefBsp.c
SRCS += RefSprite.c
SRCS += Refresh.c
SRCS += Refresh2.c
SRCS += Sight.c
SRCS += StateDef.c
SRCS += WolfIO.c
SRCS += WolfMain.c

# Object files in build directory
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
RELEASE_OBJS = $(SRCS:%.c=$(RELEASE_BUILD_DIR)/%.o)

# Target executable
TARGET = wolf3d
RELEASE_TARGET = wolf3d-release

all: $(BUILD_DIR) $(TARGET)

release: $(RELEASE_BUILD_DIR) $(RELEASE_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(RELEASE_BUILD_DIR):
	mkdir -p $(RELEASE_BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CC) $(RELEASE_OBJS) -o $(RELEASE_TARGET) $(LDFLAGS) -flto

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(RELEASE_TARGET)

.PHONY: all clean release