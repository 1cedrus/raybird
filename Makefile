default: run

build:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -Iinclude/ lib/libraylib.a main.c sewer.c -o main

run: build
	./main		

clean:
	rm main
