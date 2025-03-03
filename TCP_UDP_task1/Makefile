# Компилятор
CC = gcc

TARGET1=client_tcp
TARGET2=client_udp
TARGET3=server
LIBRARY1=
LIBRARY2=
LIBRARY3=-pthread

# Пути к исходным файлам и объектным файлам
PREF_SRC = ./create/
PREF_OBJ = ./object/

# Находим все .c файлы во всех подпапках
SRC1 = $(shell find $(PREF_SRC)$(TARGET1) -name '*.c')
SRC2 = $(shell find $(PREF_SRC)$(TARGET2) -name '*.c')
SRC3 = $(shell find $(PREF_SRC)$(TARGET3) -name '*.c')

# Генерируем список объектных файлов с уникальными именами
OBJ1 = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC1))
OBJ2 = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC2))
OBJ3 = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC3))

# Цель по умолчанию
.PHONY: all clean

# Правило для компиляции всей программы
all: $(TARGET1) $(TARGET2) $(TARGET3)

# Правило для линковки объектных файлов в исполняемый файл
$(TARGET1): $(OBJ1)
	$(CC) -o $@ $^ $(LIBRARY1)

$(TARGET2): $(OBJ2)
	$(CC) -o $@ $^ $(LIBRARY2)

$(TARGET3): $(OBJ3)
	$(CC) -o $@ $^ $(LIBRARY3)

# Правило для компиляции каждого .c файла в объектный файл
$(PREF_OBJ)%.o: $(PREF_SRC)%.c
	@mkdir -p $(dir $@)  
	$(CC) -c $< -o $@

# Очистка
clean:
	rm -rf $(PREF_OBJ) $(TARGET1) $(TARGET2) $(TARGET3)