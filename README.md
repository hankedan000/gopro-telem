# gopro-telem 
A C++ wrapper around GoPro's [gpmf-parser](https://github.com/gopro/gpmf-parser) library.

## Integration

### C++ Version
While C++17 is recommended, C++11 is the minimum version required. This library makes use of std::filesystem, and uses
[std::experimental::filesystem](https://en.cppreference.com/w/cpp/header/experimental/filesystem) if using C++11 or C++14.

### CMake Instructions
If you're including this in another CMake project, you can simply clone this repo into your project directory, 
and add the following to your CMakeLists.txt:

```cmake
# Optional: Defaults to C++ 17
# set(GOPROTELEM_CXX_STANDARD 11)
add_subdirectory(gopro-telem)

# ...

add_executable(<your program> ...)
target_link_libraries(<your program> GoProTelem::GoProTelem)
```

## Building
Pretty standard CMake build procedure.

``` bash
mkdir build
cd build
cmake ..
make -j
```

### Build with conan

``` bash
conan install . --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Testing
The repo contains a few cppunit tests. You can run them all after a build with `make test`. You should get output similar to this.

``` bash
user@dev-pc:~/git/gopro-telem/build$ make test
Running tests...
Test project /home/user/git/gopro-telem/build
    Start 1: GoProTelemTest
1/3 Test #1: GoProTelemTest ...................   Passed    0.01 sec
    Start 2: MP4_SourceTest
2/3 Test #2: MP4_SourceTest ...................   Passed    0.01 sec
    Start 3: SampleMathTest
3/3 Test #3: SampleMathTest ...................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 3

Total Test time (real) =   0.02 sec
```