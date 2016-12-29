default: build

build: align.c
	gcc align.c -o align

debug: align.c
	gcc align.c -g -o align
	gdb align

clean:
	rm -f align
