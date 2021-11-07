#nombre del ejecutable
TARGET1 = client
TARGET2 = server
TEST = test
#todos los objetos de los que dependa el ejecutable
OBJS1 = proxy.o client.o
OBJS2 = proxy.o server.o
OBJSTEST  = test.o
#flags del compilador
CFLAGS = -lpthread -Wall

.PHONY: all
all: $(TARGET1) $(TARGET2) $(TEST)

#link del target con todos los objetos
#$@ hace referencia a TARGET1
$(TARGET1) : $(OBJS1)
	gcc $(CFLAGS) $(OBJS1) -o $@

$(TARGET2) : $(OBJS2)
	gcc $(CFLAGS) $(OBJS2) -o $@

$(TEST) : $(OBJSTEST)
	gcc $(CFLAGS) $(OBJSTEST) -o $@


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
	rm -f *.o $(TARGET1) $(TARGET2)