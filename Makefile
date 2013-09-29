# Makefile Generique

EXT = c
CXX = gcc
EXEC = spidev_test

CXXFLAGS = -Wall -W -O2
LDFLAGS =


OBJDIR = obj
SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.c)
OBJ := $(addprefix ./, $(OBJ))

all: $(EXEC)

$(EXEC): $(OBJ)
	@$(CROSS_COMPILE)$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.$(EXT)
	@$(CROSS_COMPILE)$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	@rm -rf $(OBJDIR)/*.o
	@rm -f $(EXEC)

install: $(EXEC)
	@mkdir -p $(DEST_DIR)/usr/bin/
	@cp $(EXEC) $(DEST_DIR)/usr/bin/