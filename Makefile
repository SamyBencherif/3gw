
# For simplicity, I'm assuming Raylib is installed to the system

default: mac

# Debian / Ubuntu
linux:
	gcc main.c -o game -lraylib -lode -lGL -lm -lpthread -ldl -lrt -lX11

# Raspberry PI
rpi:
	gcc main.c -o game -lraylib -lode -lGL -lm -lpthread -ldl -lrt -lX11 -latomic

# Mac OS
mac:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lraylib -lode main.c -o game 