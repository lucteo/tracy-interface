from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import re
from os import path

class TracyInterfaceRecipe(ConanFile):
   name = "tracy-interface"
   version = "0.1.0"
   description = "Wrapper over the Tracy, to easily include profiling abstraction into C++ applications"
   author = "Lucian Radu Teodorescu"
   topics = ("profiling", "performance", "Tracy")
   homepage = "https://github.com/lucteo/tracy-interface"
   url = "https://github.com/lucteo/tracy-interface"
   license = "MIT"

   settings = "os", "compiler", "build_type", "arch"
   build_policy = "missing"   # Some of the dependencies don't have builds for all our targets

   options = {"shared": [True, False], "fPIC": [True, False], "with_tracy_bin": [True, False]}
   default_options = {"shared": False, "fPIC": True, "with_tracy_bin": True}

   exports = "LICENSE"
   exports_sources = ("src/*", "external/*", "CMakeLists.txt")

   def build_requirements(self):
      pass

   def config_options(self):
      if self.settings.os == "Windows":
         self.options.rm_safe("fPIC")
         # We don't build the Tracy executable on Windows
         self.options.rm_safe("with_tracy_bin")

   def configure(self):
      if self.options.shared:
         self.options.rm_safe("fPIC")

   def layout(self):
      cmake_layout(self)

   def generate(self):
      deps = CMakeDeps(self)
      deps.generate()
      tc = CMakeToolchain(self)
      tc.variables["tracy-interface.with_tracy_bin"] = "ON"
      tc.generate()

   def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()

   def package(self):
      cmake = CMake(self)
      cmake.install()

   def package_info(self):
      self.cpp_info.libs = ["tracy-interface"]


