#nombre del ejecutable
TARGET1 = p1
TARGET2 = p2
TARGET3 = p3
#todos los objetos de los que dependa el ejecutable
OBJS1 = proxy.o p1.o
OBJS2 = proxy.o p2.o
OBJS3 = proxy.o p3.o
#flags del compilador
CFLAGS = -Wall -lpthread

.PHONY: all
all: $(TARGET1) $(TARGET2) $(TARGET3)

#link del target con todos los objetos
#$@ hace referencia a TARGET1
$(TARGET1) : $(OBJS1)
	gcc $(CFLAGS) $(OBJS1) -o $@

$(TARGET2) : $(OBJS2)
	gcc $(CFLAGS) $(OBJS2) -o $@

$(TARGET3) : $(OBJS3)
	gcc $(CFLAGS) $(OBJS3) -o $@

#regla para compilar cada objeto
#ejemplo: gcc -c -Wall main.c -o main.o
#$< hace referencia a %.c
#$@ hace referencia a %.o
%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

#limpia todos los .o y el ejecutable target
#se llama con make clean
.PHONY: clean
clean:
	rm -f *.o $(TARGET1) $(TARGET2) $(TARGET3)