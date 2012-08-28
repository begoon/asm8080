all: build

build:
	cl -D_TGT_OS_WIN64=1 -Feasm8080.exe ../src/*.c strncasecmp.c

clean:
	del *.obj *.exe
