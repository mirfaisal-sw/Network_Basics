OBJS	= switch_config_util.o
SOURCE	= switch_config_util.c
OUT	= switch_config_util
CFLAGS	 += -Wall -Wextra -Wpedantic -Wstrict-prototypes -Wwrite-strings
LDFLAGS	 += -lpthread -lm -lrt

ifeq ($(PREFIX),)
    PREFIX := /usr
endif

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LDFLAGS)

switch_config_util: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

switch_config_util.o: switch_config_util.c
	$(CC) $(CFLAGS) -c switch_config_util.c 

install: switch_config_util
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 0755 switch_config_util $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f $(OBJS) $(OUT)
