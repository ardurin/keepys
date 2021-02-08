NAME = keepys
OBJECTS = main.o path.o random.o secret.o text.o

all: $(OBJECTS)
	cc -o $(NAME) $^ -lwolfssl

static: $(OBJECTS)
	cc -flto -o $(NAME) -static $^ -lwolfssl

%.o: src/%.c
	cc -c -O2 -o $@ -s -Wall $<

clean:
	rm -f $(NAME) $(OBJECTS)
