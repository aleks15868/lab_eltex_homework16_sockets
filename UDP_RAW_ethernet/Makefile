# Имена целевых программ
BIN = server client

# Компилятор
CC = gcc

# Пути к исходным файлам и объектным файлам
PREF_SRC = ./create/
PREF_OBJ = ./object/

# Исходные файлы и объектные файлы
SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

# Цель по умолчанию
.PHONY: all clean

# Правило для компиляции всех программ
all: $(BIN)

# Шаблонное правило для компиляции каждой программы
$(BIN): %: $(PREF_OBJ)%.o
	$(CC) -o $@ $<

# Правило для создания объектных файлов
$(PREF_OBJ)%.o: $(PREF_SRC)%.c
	@mkdir -p $(PREF_OBJ)  # Создать папку для объектных файлов, если её нет
	$(CC) -c $< -o $@

# Очистка
clean:
	rm -f $(BIN) $(PREF_OBJ)*.o