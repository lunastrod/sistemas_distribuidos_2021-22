#nombre del ejecutable
TARGET1 = client
TARGET2 = server
#todos los objetos de los que dependa el ejecutable
OBJS1 = client.o proxy.o
OBJS2 = server.o proxy.o
#flags del compilador
CFLAGS = -Wall -lpthread

.PHONY: all
all: $(TARGET1) $(TARGET2)

#link del target con todos los objetos
#$@ hace referencia a TARGET1
$(TARGET1) : $(OBJS1)
	gcc $(OBJS1) -o $@ $(CFLAGS)

$(TARGET2) : $(OBJS2)
	gcc $(OBJS2) -o $@ $(CFLAGS) 


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
	rm -f *.o $(TARGET1) $(TARGET2)