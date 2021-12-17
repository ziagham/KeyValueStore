# Copyright 2021 Koen van Greevenbroek         
# Subject to the GNU General Public License 3.0


CC = gcc
CFLAGS = -g -O3 -Wall -Wpedantic

all: convert

convert: src/convert.c src/base64.o
	$(CC) $(CFLAGS) -o convert src/convert.c src/base64.o

src/base64.o: src/base64.c
	$(CC) $(CFLAGS) -o src/base64.o -c src/base64.c

clean:
	-rm src/base64.o convert
