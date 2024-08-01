FLAGS="-g3 -Wno-return-type -DRENDERER_OPENGL -std=c++17 -Iinclude -Wno-unused-variable -Wno-int-to-pointer-cast"
LINK_FLAGS="/nfs/homes/zfarini/sgoinfre/homebrew/Cellar/glfw/3.4/lib/libglfw3.a"

FLAGS=$FLAGS" -g3 -fsanitize=address -fsanitize=undefined"
#FLAGS=$FLAGS" -O2"

#g++ -DRENDERER_OPENGL -std=c++17 -O2 -Iinclude code/precompiled.cpp -c -o precompiled.o
#g++ $FLAGS code/precompiled.cpp -c -o precompiled.o


#g++ -O2 -Iinclude -DRENDERER_OPENGL -c code/precompiled.cpp -o precompiled.o
g++ $FLAGS code/preprocessor.cpp -o preprocessor
g++ -DDISABLE_PREPROCESSOR $FLAGS code/game.cpp -E -o preprocessor_input.e
./preprocessor preprocessor_input.e code/generated.h
g++  $FLAGS code/glfw_main.cpp precompiled.o $LINK_FLAGS

