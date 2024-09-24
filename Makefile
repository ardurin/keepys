NAME = keepys
OBJECTS = main.o path.o random.o secret.o text.o

LIBRARIES = -lssl -lcrypto
OPTIONS =
# LIBRARIES = -lwolfssl
# OPTIONS = -D WOLFSSL

all: $(OBJECTS)
	cc -o $(NAME) $^ $(LIBRARIES)

static: $(OBJECTS)
	cc -flto -o $(NAME) -static $^ $(LIBRARIES)

%.o: src/%.c
	cc $(OPTIONS) -c -O2 -o $@ -s -Wall $<

clean:
	rm -f $(NAME) $(OBJECTS)
