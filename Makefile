BDIR = bin
SDIR = src

SRC = $(wildcard $(SDIR)/*.c)
OBJ = $(patsubst $(SDIR)/%.c,$(BDIR)/%,$(SRC))

CFLAGS += -s -O3 -std=c11 -Wall -pedantic
CPPFLAGS += -D_XOPEN_SOURCE=700

PREFIX ?= /usr/local

.PHONY: all clean install

all: $(OBJ)

$(BDIR)/%: $(SDIR)/%.c | $(BDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

$(BDIR):
	mkdir -p $@

install: $(OBJ)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	@ for target in $(OBJ); do \
		echo "$$target -> $(DESTDIR)$(PREFIX)/bin"; \
		install -m755 "$$target" $(DESTDIR)$(PREFIX)/bin; \
	done

clean:
	$(RM) -r $(BDIR)
