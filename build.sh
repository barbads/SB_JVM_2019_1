#If you're not using vcscode (you should) run this .sh to build and execute project

mkdir -p build
cd build
echo "##### Creating Makefile ######"
cmake ..
echo "#### Compiling ####"
make
cd ..
echo "#### Running `sb_tp1_2019-1`####"
./sb_tp1_2019-1
echo "Finished!"
