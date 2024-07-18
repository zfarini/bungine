FLAGS=" -std=c++17 -Iinclude -Wno-unused-variable -Wno-int-to-pointer-cast"

FLAGS=$FLAGS" -g3 -fsanitize=address -fsanitize=undefined"
#FLAGS=$FLAGS" -O2"

#g++ -DRENDERER_OPENGL -std=c++17 -O2 -Iinclude code/precompiled.cpp -c -o precompiled.o
g++ -DRENDERER_OPENGL $FLAGS code/glfw_main.cpp precompiled.o /nfs/homes/zfarini/sgoinfre/homebrew/Cellar/glfw/3.4/lib/libglfw3.a
#g++ $FLAGS main.cpp -c
