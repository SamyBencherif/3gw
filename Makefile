
# For simplicity, I'm assuming Raylib & ODE are installed to the system

default: mac

# Debian / Ubuntu
linux:
	gcc main.c -o game -lraylib -lode -lstdc++ -lGL -lm -lpthread -ldl -lrt -lX11

# Raspberry PI
rpi:
	gcc main.c -o game -lraylib -lode  -lstdc++ -lGL -lm -lpthread -ldl -lrt -lX11 -latomic

# Mac OS
mac:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lraylib -lode -lstdc++ main.c -o game 