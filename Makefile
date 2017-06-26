BUILDID = $(shell openssl rand -hex 4 | perl -ne 'print uc')


debug:
	-rm main.out
	gcc main.c -O3 -o main.out -DBUILDID="$(BUILDID)" -DDEBUG=1

all:
	-rm main.out
	gcc main.c -O3 -o main.out -DBUILDID="$(BUILDID)"

bar: all
	time ./main.out
