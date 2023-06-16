from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.system.package_manager import Apt

class goproTelemRecipe(ConanFile):
    name = "GoPromTelem"
    version = "1.0"

    # Optional metadata
    license = "<Put the package license here>"
    author = "Dan Hankewycz dany32412@gmail.com"
    url = "https://github.com/hankedan000/gopro-telem"
    description = "C++ library wrapper around GoPro's gpmf-parser library."
    topics = ("gopro", "telemetry", "video", "analysis")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["GoProTelem"]

    def requirements(self):
        self.requires("cppunit/1.15.1")

    def system_requirements(self):
        # apt = Apt(self)
        # apt.install(["libopencv-dev"], update=True, check=True)
        pass