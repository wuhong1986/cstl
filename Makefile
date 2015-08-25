CFLAGS =  -Wall
CC = gcc

cstl_test:./test/test_main.o ./test/test_clist.o ./test/test_chash.o ./src/cobj.o ./src/cobj_int.o ./src/cobj_str.o ./src/clist.o ./src/chash.o ./src/murmurhash.o ./src/cstring.o
	$(CC) $^ -g -o $@ -lcunit

%.o: %.c
	$(CC) $< -g -c -Iinclude -o $@ $(CFLAGS)

clean:
	rm -f cstl_test *.o

.PHONY: clean
