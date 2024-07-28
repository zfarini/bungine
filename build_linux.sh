FLAGS="-Wno-return-type -DRENDERER_OPENGL -std=c++17 -Iinclude -Wno-unused-variable -Wno-int-to-pointer-cast"

#FLAGS=$FLAGS" -g3 -fsanitize=address -fsanitize=undefined"
#FLAGS=$FLAGS" -O2"

#g++ -DRENDERER_OPENGL -std=c++17 -O2 -Iinclude code/precompiled.cpp -c -o precompiled.o
g++ $FLAGS code/glfw_main.cpp code/precompiled.cpp -lm -lX11 -lEGL
#g++ $FLAGS main.cpp -c
