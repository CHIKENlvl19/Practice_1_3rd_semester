CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = 


SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = dbms


INCLUDE_DIR = include
DATA_DIR = data

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


dirs:
	@mkdir -p $(DATA_DIR)

run: $(TARGET) dirs
	./$(TARGET) --file $(DATA_DIR)/test.data --query 'HSET myhash key value'

test: $(TARGET) dirs
	./$(TARGET) --file $(DATA_DIR)/test.data --query '$(QUERY)'


clean:
	rm -f $(OBJECTS) dbms *.data

clean-all: clean
	rm -f $(TARGET)
	rm -rf $(DATA_DIR)/*.data


help:
	@echo "Доступные цели:"
	@echo "  make              - скомпилировать проект"
	@echo "  make run          - скомпилировать и запустить пример"
	@echo "  make test QUERY=... - скомпилировать и запустить с кастомной командой"
	@echo "  make clean        - удалить объектные файлы"
	@echo "  make clean-all    - удалить всё включая исполняемый файл"
	@echo "  make help         - показать эту справку"
	@echo ""
	@echo "Примеры использования:"
	@echo "  make test QUERY='HSET hash1 key1 value1'"
	@echo "  make test QUERY='HGET hash1 key1'"
	@echo "  make test QUERY='SADD myset apple'"
	@echo "  make test QUERY='SISMEMBER myset apple'"

.PHONY: all run test clean clean-all dirs help
