#!/bin/bash
recurse() {
	for i in "$1"/*.cpp;do
		if [ -f "$i" ]; then
			printf "${i} $files"
		fi
	done
	for i in "$1"/*;do
		if [ -d "$i" ];then
			recurse "$i"
		fi
	done
}

SRC=$(recurse .)
OUT=./bin/out

echo '========='

echo $SRC

echo '========='

g++ $SRC -o $OUT glad.o -lglfw -ldl

echo $OUT