T0 = simple_join_selinger
T1 = naive
T5 = simple_join

CC = g++
CFLAGS = -g -std=c++17 -c -Wall -O2
LDFLAGS =

OBJ_T0 = simple_join_selinger.o
OBJ_T1 = naive.o
OBJ_T5 = simple_join.o

all: $(T0) $(T1) $(T5)
# $(T6) $(T3) $(T4)

$(T0): $(OBJ_T0)
	$(CC) $(LDFLAGS) -o $@ $^

$(T1): $(OBJ_T1)
	$(CC) $(LDFLAGS) -o $@ $^

# $(T2): $(OBJ_T2)
# 	$(CC) $(LDFLAGS) -o $@ $^

# $(T3): $(OBJ_T3)
# 	$(CC) $(LDFLAGS) -o $@ $^

# $(T4): $(OBJ_T4)
# 	$(CC) $(LDFLAGS) -o $@ $^

$(T5): $(OBJ_T5)
	$(CC) $(LDFLAGS) -o $@ $^

# $(T5): $(OBJ_T6)
# 	$(CC) $(LDFLAGS) -o $@ $^

.cc.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.o *.exe *.stackdump $(T0) $(T1) $(T5)
	# $(T6) $(T3) $(T4)
