CXX:=g++
CFLAGS:=-O3 -Wall -Wextra -Iinclude -g

SRC:=$(wildcard src/*.cpp)
OBJ:=$(SRC:.cpp=.o)
DEP:=$(SRC:.cpp=.d)
OUT:=snmp

all: $(OUT)

-include $(OBJ:.o=.d)

$(OUT): $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $< > $*.d

.PHONY: clean

clean:
	rm -f $(OUT) $(OBJ) $(DEP)
