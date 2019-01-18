#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2018 Bincrafters

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

from conans import ConanFile, tools
import os

class GraphConan(ConanFile):
    name = "graph"
    version = "0.1.0"
    description = "Efficient, header-only graph library for C++17 with a pleasant interface."
    topics = ("graph-algorithms", "generic", "graph")
    url = "https://github.com/cbbowen/graph/"
    homepage = "https://github.com/cbbowen/graph/"
    author = "Chris Bowen"
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
