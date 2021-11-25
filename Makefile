CC = g++
CFLAGS = -Wall -O3
LIB = -lpthread -lpoet -lhb-acc-pow-shared -lhb-energy-msr -lm
EXE = bench_client
DEPS = KV-engine/dummy-keystore.c

all: $(EXE)

$(EXE): bench_client.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(DEPS) $(LIB)

.PHONY: clean
clean:
	-rm $(EXE)