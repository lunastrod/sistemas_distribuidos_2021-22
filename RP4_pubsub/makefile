#nombre del ejecutable
TARGET1 = publisher
TARGET2 = subscriber
TARGET3 = broker
#todos los objetos de los que dependa el ejecutable
OBJS1 = publisher.o proxy.o client_list.o
OBJS2 = subscriber.o proxy.o client_list.o
OBJS3 = broker.o proxy.o client_list.o
OBJS_TEST = proxy.o client_list.o
#flags del compilador
CFLAGS = -Wall -lpthread

.PHONY: all
all: $(TARGET1) $(TARGET2) $(TARGET3)

#link del target con todos los objetos
#$@ hace referencia a TARGET1
#$^ hace referencia a OBJS1
$(TARGET1) : $(OBJS1)
	gcc $^ -o $@ $(CFLAGS)

$(TARGET2) : $(OBJS2)
	gcc $^ -o $@ $(CFLAGS)

$(TARGET3) : $(OBJS3)
	gcc $^ -o $@ $(CFLAGS)

#regla para compilar cada objeto
#ejemplo: gcc -c -Wall main.c -o main.o
#$< hace referencia a %.c
#$@ hace referencia a %.o
%.o : %.c
	gcc -c $< -o $@ $(CFLAGS)

#limpia todos los .o y el ejecutable target
#se llama con make clean
.PHONY: clean
clean:
	clear
	rm -f *.o $(TARGET1) $(TARGET2) $(TARGET3)

#compila de 0 y elimina los .o
.PHONY: fresh
fresh: | clean all
	rm -f *.o

.PHONY: test
test: | all
	gcc -c tests/test.c -o tests/test.o $(CFLAGS) -I. 
	gcc tests/test.o $(OBJS_TEST) -o tests/test $(CFLAGS) -lcheck -lpthread -lrt -lm -lsubunit
	./tests/test
