.PHONY: all prepare build example

all: prepare build

prepare:
	@rm -rf build
	@mkdir build

build:
	@cmake -S . -B build
	@cmake --build build

example: prepare
	@cmake -DARGS_PARSER_BUILD_EXAMPLE=ON -S . -B build
	@cmake --build build
	./build/bin/args_parser_example --help
	./build/bin/args_parser_example data.txt --port=8080 --verbose
