from conans import ConanFile, CMake, tools

class GraphConan(ConanFile):
    name = "Graph"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    exports_sources = "include/*", "CMakeLists.txt" #, "test/*"
    no_copy_source = True

    requires = "range-v3/0.4.0@ericniebler/stable"
    build_requires = "Catch2/2.5.0@catchorg/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        # if tools.get_env("CONAN_RUN_TESTS", True):
        #     cmake.test()

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
