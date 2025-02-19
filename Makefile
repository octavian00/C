TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f mydatabse.db -n
	./$(TARGET) -f mydatabse.db -a "Timmy H,123 Giroc,120"
	./$(TARGET) -f mydatabse.db -l
default: $(TARGET)

clean:
	rm -rf obj/*.o
	rm -rf bin/*
	rm -rf *.db

# Ensure bin and obj directories exist
$(TARGET): $(OBJ)
	@mkdir -p bin
	gcc -o $@ $^

obj/%.o : src/%.c
	@mkdir -p obj
	gcc -c $< -o $@ -Iinclude
