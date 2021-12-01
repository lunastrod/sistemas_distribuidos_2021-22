#nombre del ejecutable
TARGET1 = broker
TARGET2 = publisher
TARGET3 = subscriber
#todos los objetos de los que dependa el ejecutable
O_DIR = obj/
OBJS1 = $(O_DIR)proxy.o $(O_DIR)client_list.o $(O_DIR)broker.o
OBJS2 = $(O_DIR)proxy.o $(O_DIR)client_list.o $(O_DIR)publisher.o
OBJS3 = $(O_DIR)proxy.o $(O_DIR)client_list.o $(O_DIR)subscriber.o
#flags del compilador
CFLAGS = -Wall -lpthread

.PHONY: all

all: $(TARGET1) $(TARGET2) $(TARGET3)

#link del target con todos los objetos
#$@ hace referencia a TARGET1
$(TARGET1) : $(OBJS1)
	gcc $(OBJS1) -o $@ $(CFLAGS)

$(TARGET2) : $(OBJS2)
	gcc $(OBJS2) -o $@ $(CFLAGS) 

$(TARGET3) : $(OBJS3)
	gcc $(OBJS3) -o $@ $(CFLAGS)

#regla para compilar cada objeto
#ejemplo: gcc -c -Wall main.c -o main.o
#$< hace referencia a %.c
#$@ hace referencia a %.o
#la @ delante de un comando hace que no se imprima
$(O_DIR)%.o : %.c
	@mkdir -p $(O_DIR)
	gcc -c $< -o $@ $(CFLAGS)

#limpia todos los .o y el ejecutable target
#se llama con make clean
.PHONY: clean
clean:
	rm -f $(O_DIR)*.o $(TARGET1) $(TARGET2) $(TARGET3)
	rm -d $(O_DIR)