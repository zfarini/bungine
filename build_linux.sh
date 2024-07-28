FLAGS="-g3 -Wno-return-type -DRENDERER_OPENGL -std=c++17 -Iinclude -Wno-unused-variable -Wno-int-to-pointer-cast"
LINK_FLAGS="/nfs/homes/zfarini/sgoinfre/homebrew/Cellar/glfw/3.4/lib/libglfw3.a"

#FLAGS=$FLAGS" -g3 -fsanitize=address -fsanitize=undefined"
#FLAGS=$FLAGS" -O2"

#g++ -DRENDERER_OPENGL -std=c++17 -O2 -Iinclude code/precompiled.cpp -c -o precompiled.o
#g++ $FLAGS code/precompiled.cpp -c -o precompiled.o


g++ -Iinclude -DRENDERER_OPENGL -c code/precompiled.cpp -fPIC -o precompiled.o

value=$(od -A n -t d -N 1 /dev/urandom | tr -d ' ')
name="game.so$value"
rm -f game.so*
#TODO: don't link with glfw here
g++ -shared -o tmp_$name $FLAGS code/game.cpp precompiled.o -fPIC $LINK_FLAGS
mv tmp_$name $name

g++  $FLAGS code/glfw_main.cpp precompiled.o $LINK_FLAGS
