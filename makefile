default: build

build: align.c
	gcc align.c -o align

debug: align.c
	gcc align.c -g -o align
	gdb align

test: align.c
	gcc align.c -g -o align
	gcc tests/align_Runner.c -o test
	./test
	bats tests/tests.bat

clean:
	rm -f align
