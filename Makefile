# --- Windows variables ---
CC_WIN = g++
CFLAGS_WIN = -Wall -Wextra -D_WIN32 -Iinclude -g
TARGET_WIN = obd_scanner.exe
SRC_WIN = src/main.cpp lib/com.cpp lib/obd2/pid_map.c pid_decoders.c elm327_at.c

# --- ESP32 variables ---
CLI = arduino-cli
FQBN = esp32:esp32:esp32dev
PORT = COM5  # On Linux, for example /dev/ttyUSB0
SRC_ESP = src/

.PHONY: all win esp flash clean

# Default target (builds the Windows version)
all: win

# 1. Windows target (native C compilation)
win:
	@echo "Compiling for Windows..."
	$(CC_WIN) $(CFLAGS_WIN) $(SRC_WIN) -o $(TARGET_WIN)
	@echo "Done: $(TARGET_WIN)"

# 2. ESP32 target (via arduino-cli)
esp:
	@echo "Compiling for ESP32..."
	$(CLI) compile --fqbn $(FQBN) $(SRC_ESP)
	@echo "ESP32 compilation successful."

# 3. Target for uploading to ESP32
flash: esp
	@echo "Flashing to ESP32 on port $(PORT)..."
	$(CLI) upload -p $(PORT) --fqbn $(FQBN) $(SRC_ESP)
	@echo "ESP32 flashed."

# 4. Cleanup
clean:
	@echo "Deleting build files..."
	del /q /f $(TARGET_WIN)
	# arduino-cli manages its cache in temporary folders,
	# but you can clear the cache explicitly to be safe:
	# CLI cache clean