
# Usage
#
# Build:
#	make
# Build with optimizations:
#	make CONFIG=release
# Debug with gdb:
#	make run
# Profile with valgrind:
#	make CONFIG=profile run
# Clean:
#	make clean
# Clean all configurations:
#	make distclean

#############
## Options ##
#############

# OPTION: Name the output binary
NAME ?= $(notdir $(shell pwd))

# OPTION: Add additional compiler, preprocessor, and loader flags
CXXFLAGS += -std=c++1z
CPPFLAGS +=
LDFLAGS +=

# OPTION: Add additional source directories and extensions
SRCDIRS = .
SRCEXTS = .cpp

# OPTION: Add additional header directories and extensions
HDRDIRS = $(SRCDIRS) ../include ../extern/ranges-v3/include ../extern/catch2/single_include
HDREXTS = .h .hpp

# OPTION: Add additional libary directories and libaries
LIBDIRS =
LIBS =

# OPTION: Change the compiler
#CXX = clang++

# OPTION: Change flags for different configurations
DEBUGFLAGS = -g -O0
RELEASEFLAGS = -O2 -DNDEBUG -DGRAPH_BENCHMARK
PROFILEFLAGS = -g $(RELEASEFLAGS) -DGRAPH_BENCHMARK
GCOVFLAGS = -g -O0 -fprofile-arcs -ftest-coverage --coverage # For gcov
LLVMCOVFLAGS = -fprofile-instr-generate -fcoverage-mapping # For llvm-cov

###########
## Paths ##
###########

# Filter out non-existant directories
EXISTINGSRCDIRS = $(call __where_dir_exists,$(SRCDIRS))
EXISTINGHDRDIRS = $(call __where_dir_exists,$(HDRDIRS))
EXISTINGLIBDIRS = $(call __where_dir_exists,$(LIBDIRS))

# Find all the source files
SOURCES = $(foreach d,$(EXISTINGSRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

# Directory for all build files
# Note: in-tree builds are not supported
ALLBUILDDIR ?= build

# Default to the debug configuration
# Note: everything before this must be configuration agnostic
CONFIG ?= release

# Build directory
BUILDDIR = $(ALLBUILDDIR)/$(CONFIG)

# Object files
OBJDIR = $(BUILDDIR)/obj
OBJECTS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SOURCES))))
DEPENDS = $(OBJECTS:.o=.d)

# Binary files
BINDIR = $(BUILDDIR)/bin
OUTPUT = $(BINDIR)/$(NAME)

###########
## Flags ##
###########

# Select the config flags based on the current configuration
ifeq ($(CONFIG),debug)
	CONFIGFLAGS = $(DEBUGFLAGS)
else ifeq ($(CONFIG),release)
	CONFIGFLAGS = $(RELEASEFLAGS)
else ifeq ($(CONFIG),profile)
	CONFIGFLAGS = $(PROFILEFLAGS)
else ifeq ($(CONFIG),gcov)
	CONFIGFLAGS = $(GCOVFLAGS)
else ifeq ($(CONFIG),llvm-cov)
	CONFIGFLAGS = $(LLVMCOVFLAGS)
endif

# Add the config flags to the compiler flags
CXXFLAGS += $(CONFIGFLAGS)

# Generate the appropriate compiler and linker flags based on headers and libraries
HDRFLAGS = $(foreach d,$(EXISTINGHDRDIRS),-I$(d))
LIBFLAGS = $(call __concat, \
	$(foreach d,$(EXISTINGLIBDIRS),-L$(d)) \
	$(foreach d,$(LIBS),-l$(d)))

# Flags for profiling (with callgrind)
PROFILEOUTPUT = $(BINDIR)/callgrind.out
PROFILEANALYZEFLAGS = $(call __concat, \
	$(foreach d,$(EXISTINGSRCDIRS),--include=$(d)) \
	$(foreach d,$(EXISTINGHDRDIRS),--include=$(d)))
#PROFILEANALYZEFLAGS += --tree=calling # Uncomment to include calling function in profile information

# Flags used to generate automatic dependencies
DEPFLAGS += -MD -MP

##############
## Commands ##
##############

# Commands for compiling, linking, and cleaning
MKDIR = @mkdir -p
RM = @rm -f
RMDIR = @rm -rf --one-file-system
COMPILE = $(CXX) $(DEPFLAGS) $(HDRFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c
LINK = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
DEBUGGER = gdb -ex run -ex quit --quiet --return-child-result
PROFILER = valgrind --tool=callgrind --callgrind-out-file=$(PROFILEOUTPUT)
PROFILE_ANALYZER = callgrind_annotate $(PROFILEANALYZEFLAGS)

##############
## INTERNAL ##
##############

__concat = $(foreach __concat_s,$(1),$(__concat_s))
__canonical_dir = $(patsubst %/,%,$(patsubst %/.,%,$(1)))
__where_dir_exists = $(call __canonical_dir,$(wildcard $(1)/.))
__where_in_dir = $(filter $(realpath $(1))/%,$(realpath $(2))/)
__makefile = $(lastword $(MAKEFILE_LIST))

#############
## Targets ##
#############

# OPTION: Add anything else that should be done when building here
all: $(OUTPUT)

# Targets for object files
$(OBJDIR)/%.o:%.cpp
	$(MKDIR) $(dir $@)
	$(COMPILE) $< -o $@

# Target for output binary
$(OUTPUT): Makefile $(OBJECTS)
	$(MKDIR) $(dir $@)
	$(LINK) $(OBJECTS) $(LIBFLAGS) -o $@

# Target for running the resulting binary
run: all
ifeq ($(CONFIG),debug)
	$(DEBUGGER) $(OUTPUT)
else ifeq ($(CONFIG),release)
	$(OUTPUT)
else ifeq ($(CONFIG),profile)
	$(PROFILER) $(OUTPUT)
	$(PROFILE_ANALYZER) $(PROFILEOUTPUT)
else ifeq ($(CONFIG),gcov)
	rm $(OBJDIR)/*.gcda
	$(OUTPUT)
	lcov --base-directory $(shell pwd) --directory $(OBJDIR) --gcov-tool ./llvm-gcov.sh --capture --output-file $(BUILDDIR)/unfiltered.info
	lcov --extract $(BUILDDIR)/unfiltered.info $(shell readlink -f ../include)/\* -o $(BUILDDIR)/coverage.info
	genhtml $(BUILDDIR)/coverage.info -o $(BUILDDIR)
else ifeq ($(CONFIG),llvm-cov)
	$(OUTPUT)
	mv default.profraw $(BUILDDIR)
	llvm-profdata merge $(BUILDDIR)/default.profraw -o $(BUILDDIR)/default.profdata
	llvm-cov show $(OUTPUT) -format=html -output-dir=$(BUILDDIR) -instr-profile=$(BUILDDIR)/default.profdata ../include
else
	$(warning No run command for configuration $(CONFIG))
endif

# Removes the build files for the current configuration
clean:
	$(RMDIR) $(BUILDDIR)

# Removes temp files created by many editors
cleantemp:
	$(RM) $(wildcard ./*~)

# Puts the project in a state ready for distribution by removing all build and temp files
distclean: cleantemp
ifneq ($(call __where_in_dir,$(ALLBUILDDIR),$(__makefile)),)
	$(error ALLBUILDDIR ($(ALLBUILDDIR)) contains $(__makefile)')
else
	$(RMDIR) $(ALLBUILDDIR)
endif

# Show variables useful for diagnosing configuration problems
show:
	@echo "SOURCES = $(SOURCES)"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "DEPENDS = $(DEPENDS)"
	@echo "CONFIG = $(CONFIG)"
	@echo "OUTPUT = $(OUTPUT)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "HDRFLAGS = $(HDRFLAGS)"
	@echo "LIBFLAGS = $(LIBFLAGS)"
	@echo "CPPFLAGS = $(CPPFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "BUILDDIR = $(BUILDDIR)"
	@echo "OBJDIR = $(OBJDIR)"
	@echo "BINDIR = $(BINDIR)"

# Include the dependencies generated by the compiler unless NODEPS is defined
ifndef NODEPS
ifneq ($(DEPENDS),)
sinclude $(DEPENDS)
endif
endif
