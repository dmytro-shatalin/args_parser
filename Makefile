.PHONY: all prepare build

all: prepare build

prepare:
	rm -rf build
	mkdir build
	cmake -S . -B build

build:
	@cmake --build build