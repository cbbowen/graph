#!/bin/bash
if [ $1 == "-v" ] ; then
	# What, pray tell, is this abomination?  Clang outputs ancient .gcda coverage files.  Fortunately, it provides its own version of gcov for this reason.  But lcov doesn't know how to use this tool, so we trick it into thinking it has a gcov with the correct version.
	echo "gcov () 4.2.0"
else
	exec llvm-cov gcov "$@"
fi
