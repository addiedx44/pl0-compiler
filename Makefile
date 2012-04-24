# set up directories
INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

# linux commands/flags
CC = gcc
CFLAGS = -Wall -I $(INCDIR)
RM = rm

# files
EXE = $(BINDIR)/pl0-compiler

_OBJS = pl0-compiler.o pl0-lex.o pl0-parsegen.o pl0-tokens.o pm0.o fancy_string.o lexeme_list.o
OBJS = $(patsubst %, $(OBJDIR)/%, $(_OBJS))

# recipes
all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) -f $(OBJS)

spotless: clean
	$(RM) -f $(EXE)
