SHELL=/bin/bash
source := $(wildcard color_tagger*.cpp)
out := $(source:%.cpp=%)


all: $(out)
.PHONY: all

%: %.cpp
	g++ -std=c++11 $< -o $@ `pkg-config --cflags --libs opencv`


clean:
	chmod +w $(out)
	rm $(out)
.PHONY: clean

install: $(out) 
	cp -t /usr/local/bin $(notdir $(out))
.PHONY: install

uninstall:
	rm $(addprefix /usr/local/bin/, $(out))
.PHONY: uninstall

