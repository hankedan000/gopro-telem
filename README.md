# gopro-telem 
A C++ wrapper around GoPro's [gpmf-parser](https://github.com/gopro/gpmf-parser) library.

# Building
Pretty standard CMake build procedure.

``` bash
mkdir build
cd build
cmake ..
make -j
```

# Testing
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