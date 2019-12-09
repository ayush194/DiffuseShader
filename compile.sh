#!/bin/sh

if [[ $(uname) =~ "Darwin" ]]
then
	g++ -std=c++11 -framework OpenGL -lglfw -lglew -I/usr/local/include -I. -o diffuseshader main.cpp App.cpp Object.cpp objloader.cpp
elif [[ $(uname) =~ "Linux" ]]
then
	g++ -std=c++11 -I/usr/local/include -I. -o diffuseshader main.cpp App.cpp Object.cpp objloader.cpp -lglfw -lGLEW -lGL -lGLU
else
	echo "This build is not supported for Operating Systems other than Linux and OSX!"
fi
