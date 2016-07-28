SHELL=/bin/bash
CC := g++
SOURCE := $(wildcard color_tagger*.cpp)
OUT := $(SOURCE:%.cpp=%)
COMPILER_FLAGS := -std=c++11 -O3


all: $(OUT)
.PHONY: all

%: %.cpp
	$(CC) $(COMPILER_FLAGS) $< -o $@ `pkg-config --cflags --libs opencv`

clean:
	chmod +w $(OUT)
	rm $(OUT)
.PHONY: clean

install: $(OUT) 
	cp -t /usr/local/bin $(notdir $(OUT))
.PHONY: install

uninstall:
	rm $(addprefix /usr/local/bin/, $(OUT))
.PHONY: uninstall
