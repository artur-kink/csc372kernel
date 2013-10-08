
SOURCES= kernel.c \
	list.c \
	user.c \
	main.c
OBJECTS=$(SOURCES:.c=.o)

all: $(OBJECTS)
	gcc -o lab3 $(OBJECTS)

$(OBJECTS): %.o: %.c
	gcc $@.d -o $@ $<
