CFLAGS=-fpic

all: hello.so

%.so : %.c
		$(CC) $(CFLAGS) $(LDFLAGS) -shared $< -o $@

