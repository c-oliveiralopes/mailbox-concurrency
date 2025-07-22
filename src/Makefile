# Makefile para o Sistema de Mailbox Concorrente

# Variáveis
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
TARGET = mailbox
SRCDIR = .
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

# Regra principal
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS)
	@echo "Build concluído: $(TARGET)"

# Compilação dos objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Arquivos limpos"

# Reinstalação completa
rebuild: clean all

# Execução do programa
run: $(TARGET)
	./$(TARGET)

# Verificação de vazamentos de memória (requer valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Instalação
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "$(TARGET) instalado em /usr/local/bin/"

# Desinstalação
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "$(TARGET) removido de /usr/local/bin/"

# Help
help:
	@echo "Comandos disponíveis:"
	@echo "  all      - Compila o projeto (padrão)"
	@echo "  clean    - Remove arquivos gerados"
	@echo "  rebuild  - Limpa e recompila"
	@echo "  run      - Compila e executa o programa"
	@echo "  debug    - Compila com informações de debug"
	@echo "  release  - Compila otimizado para release"
	@echo "  memcheck - Executa verificação de memória com valgrind"
	@echo "  install  - Instala o programa no sistema"
	@echo "  help     - Mostra esta ajuda"

# Declara targets que não são arquivos
.PHONY: all clean rebuild run memcheck debug release install uninstall help