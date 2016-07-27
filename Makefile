SHELL=/bin/bash
CC := g++
SOURCE := $(wildcard color_tagger*.cpp)
OUT := $(SOURCE:%.cpp=%)


all: $(OUT)
.PHONY: all

%: %.cpp
	$(CC) -std=c++11 $< -o $@ `pkg-config --cflags --libs opencv`

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
