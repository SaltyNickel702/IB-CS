rm -rf build && mkdir build
cd build
if [[ "$OSTYPE" == "darwin"* ]]; then
    cmake ..

elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
	cmake -G "Ninja" ..
fi

cd ..
printf "\nCompiling: \n"
sh comp.sh