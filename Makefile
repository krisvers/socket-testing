all:
	clang++ $(shell find . -type f -name "*.cpp") -o networking