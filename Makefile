NAME = keepys
OBJECTS = main.o path.o random.o secret.o text.o

LIBRARIES = -lssl -lcrypto
OPTIONS =
# LIBRARIES = -lwolfssl
# OPTIONS = -D WOLFSSL

all: $(OBJECTS)
	cc -flto -o $(NAME) $^ $(LIBRARIES)

static: $(OBJECTS)
	cc -flto -o $(NAME) -static $^ $(LIBRARIES)

%.o: src/%.c
	cc $(OPTIONS) -c -flto -O2 -o $@ -s -Wall -Wextra $<

clean:
	rm -f $(NAME) $(OBJECTS)
