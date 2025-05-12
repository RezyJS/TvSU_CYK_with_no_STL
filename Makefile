# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Iinclude -Wall -Wextra -O2

# Пути
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Файлы
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Имя итогового исполняемого файла
TARGET = cyk_parser

# Правило по умолчанию
all: $(TARGET)

# Компиляция объекта
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Создание папки для объектов, если её нет
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Линковка
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
