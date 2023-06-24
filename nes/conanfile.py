from conan import ConanFile


class Conan(ConanFile):
    settings = "os", "compiler", "arch", "build_type"
    requires = ["sfml/2.5.1"]
    generators = "cmake"