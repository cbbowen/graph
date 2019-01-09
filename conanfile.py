#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, tools
import os

class GraphConan(ConanFile):
    name = "graph"
    version = "0.1.0"
    description = "Efficient, header-only graph library for C++17 with a pleasant interface."
    topics = ("graph-algorithms", "generic", "graph")
    url = "https://github.com/cbbowen/graph/"
    homepage = "https://github.com/cbbowen/graph/"
    author = "Christian Bowen"
    license = "MIT"
    no_copy_source = True

    # Packages the license for the conanfile.py
    exports = ["LICENSE.md"]

    # Requirements
    requires = "range-v3/0.4.0@ericniebler/stable"

    # Custom attributes for Bincrafters recipe conventions
    _source_subfolder = "source_subfolder"

    def source(self):
        source_url = "https://github.com/cbbowen/graph/"
        tools.get("{0}/archive/v{1}.tar.gz".format(source_url, self.version), sha256="Please-provide-a-checksum")
        extracted_dir = self.name + "-" + self.version

        #Rename to "source_folder" is a convention to simplify later steps
        os.rename(extracted_dir, self._source_subfolder)

    def package(self):
        include_folder = os.path.join(self._source_subfolder, "include")
        self.copy(pattern="LICENSE", dst="licenses", src=self._source_subfolder)
        self.copy(pattern="include/*", dst="include", src=include_folder)

    def package_id(self):
        self.info.header_only()
