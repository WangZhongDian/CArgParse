from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
import os


class loggingRecipe(ConanFile):
    name = "cargparse"
    version = "0.3.1"
    license = "MIT"
    author = "321640253@qq.com"
    url = "https://gitea.youmetme.wang/youmetme/logging"
    description = "C语言命令行参数解析器"
    topics = (
        "argparse",
        "C",
        "simple",
        "easy-to-use",
        "CArgParse",
        "args",
        "cargparse",
    )

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "test": [True, False],
        "example": [True, False],
    }
    default_options = {"shared": False, "fPIC": True, "test": True, "example": False}

    exports_sources = "include/*", "CMakeLists.txt", "src/*", "tests/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["TEST"] = True if self.options.test else False
        tc.variables["SHARED_BUILD"] = True if self.options.shared else False
        tc.variables["EXAMPLE"] = True if self.options.example else False
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.test:
            cmake.test()

    def package(self):
        copy(
            self,
            "LICENSE",
            src=self.source_folder,
            dst=os.path.join(self.package_folder, "licenses"),
        )
        copy(
            self,
            pattern="*.h",
            src=os.path.join(self.source_folder, "include"),
            dst=os.path.join(self.package_folder, "include"),
        )
        copy(
            self,
            pattern="*.a",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.so",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.lib",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.dll",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "bin"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.dylib",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )

    def package_info(self):
        self.cpp_info.libs = ["cargparse"]
