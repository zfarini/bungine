#!/bin/bash
ROOT_DIR="$(dirname "$(realpath "$0")")"

COMPILER="clang++"
COMMON_FLAGS="-DPLATFORM_LINUX -DRENDERER_OPENGL -std=c++17 -I$ROOT_DIR/include "
FLAGS=$COMMON_FLAGS" -Wno-return-type -Wno-unused-variable -Wno-int-to-void-pointer-cast"
LINK_FLAGS="-L$ROOT_DIR/lib -lglfw3 -lpthread"


if [ ! -d $ROOT_DIR/build ]; then
  mkdir $ROOT_DIR/build
fi

cd "$ROOT_DIR/build"

#FLAGS=$FLAGS" -g3 -fsanitize=address -fsanitize=undefined"
FLAGS=$FLAGS" -O3"

if [ ! -e precompiled.o ]; then
echo "precompiled.o not found, recompiling..."
$COMPILER -O2 $COMMON_FLAGS $ROOT_DIR/code/precompiled.cpp -c -o precompiled.o
fi

$COMPILER $COMMON_FLAGS $ROOT_DIR/code/preprocessor.cpp -o preprocessor
$COMPILER -DDISABLE_PREPROCESSOR $COMMON_FLAGS $ROOT_DIR/code/game.cpp -E -o preprocessor_input.e
./preprocessor preprocessor_input.e .generated.h
if which clang-format >/dev/null 2>&1; then
	clang-format .generated.h > $ROOT_DIR/code/generated.h
else 
	cp .generated.h $ROOT_DIR/code/generated.h
fi
rm -f .generated.h

$COMPILER $FLAGS $ROOT_DIR/code/glfw_main.cpp precompiled.o $LINK_FLAGS -o $ROOT_DIR/game

