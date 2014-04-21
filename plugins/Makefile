CFLAGS=-fpic

all: hello.so python.so

python.so: python.c
		$(CC) $(CFLAGS) $(LDFLAGS) -lpython -shared $< -o $@

%.so : %.c
		$(CC) $(CFLAGS) $(LDFLAGS) -shared $< -o $@

