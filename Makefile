CC      := gcc
CFLAGS  := -O3

SOURCES := src

CFILES  := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
HEADERS := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.h))

main: $(CFILES) $(HEADERS)
	$(CC) $(CFLAGS) $(CFILES) -o $@
