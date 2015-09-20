Build instructions. If something doesn't work, look at CMakeLists.txt, figure it out and fix it; or just let me know of the problem.


Windows:

To build with Visual Studio 2013:
 - Create directory src/build
 - From inside it run: cmake .. -G"Visual Studio 12 2013 Win64"
   Without the -G argument it will generate 32-bit project which fails to build.
 - Build the project.
 - To be able to run from IDE:
   -- change fract->PROJECT->Properties->Debugging->Working Directory to "$(ProjectDir).."
   -- right click on "fract" project, select "Set as StartUp Project"

Building with other versions of VS or MinGW is most likely straightforward but I didn't try.


Mac OS:

build:

cd src
mkdir build
cd build
cmake ..
make

run:

cd src
build/fract
