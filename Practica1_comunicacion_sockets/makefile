
simple_client: simple_client.c
	gcc -o simple_client simple_client.c

simple_server: simple_server.c
	gcc -o simple_server simple_server.c

nonblock_client: nonblock_client.c
	gcc -o nonblock_client nonblock_client.c

nonblock_server: nonblock_server.c
	gcc -o nonblock_server nonblock_server.c

complex_server: complex_server.c
	gcc -o complex_server complex_server.c -lpthread

modsimple_client: modsimple_client.c
	gcc -o modsimple_client modsimple_client.c

all: nonblock_server nonblock_client simple_server simple_client complex_server modsimple_client