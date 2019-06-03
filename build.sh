#If you're not using vcscode (you should) run this .sh to build and execute project
mkdir -p build

cd build

echo "##### Creating Makefile ######"

cmake ..

echo "#### Compiling ####"

make

cd ..

echo "Finished build! Execute calling ./sb-2019 program.class"
