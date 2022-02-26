from conans import ConanFile, CMake, tools
import re
from os import path

class TracyInterfaceRecipe(ConanFile):
   name = "tracy-interface"
   description = "Wrapper over the Tracy, to easily include profiling abstraction into C++ applications"
   author = "Lucian Radu Teodorescu"
   topics = ("profiling", "performance", "Tracy")
   homepage = "https://github.com/lucteo/tracy-interface"
   url = "https://github.com/lucteo/tracy-interface"
   license = "MIT"

   settings = "os", "compiler", "build_type", "arch"
   generators = "cmake"
   build_policy = "missing"   # Some of the dependencies don't have builds for all our targets

   options = {"shared": [True, False], "fPIC": [True, False], "with_profiling": [True, False], "with_tracy_bin": [True, False]}
   default_options = {"shared": False, "fPIC": True, "with_profiling": True, "with_tracy_bin": True}

   exports = "LICENSE"
   exports_sources = ("src/*", "CMakeLists.txt")

   def set_version(self):
      # Get the version from src/CMakeList.txt project definition
      content = tools.load(path.join(self.recipe_folder, "src/CMakeLists.txt"))
      version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
      self.version = version.strip()

   def build_requirements(self):
      pass

   def config_options(self):
       if self.settings.os == "Windows":
           del self.options.fPIC
           # We don't build the Tracy executable on Windows
           del self.options.with_tracy_bin

   def build(self):
      # Note: options "shared" and "fPIC" are automatically handled in CMake
      cmake = self._configure_cmake()
      cmake.build()

   def package(self):
      self.copy(pattern="LICENSE", dst="licenses")
      cmake = self._configure_cmake()
      cmake.install()

   def package_info(self):
      self.cpp_info.libs = self.collect_libs()

   def _configure_cmake(self):
      cmake = CMake(self)
      if self.settings.compiler == "Visual Studio" and self.options.shared:
         cmake.definitions["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = True
      if self.options.with_profiling:
         cmake.definitions["tracy-interface.profiling"] = "ON"
      if self.options.with_tracy_bin:
         cmake.definitions["tracy-interface.with_tracy_bin"] = "ON"

      cmake.configure(source_folder="src")
      return cmake


