
# OPTION: Add additional compiler, preprocessor, and loader flags
OMPFLAGS ?= -fopenmp
CXXFLAGS += -std=c++1z $(OMPFLAGS)
LDFLAGS +=

# OPTION: Add additional header directories and extensions
HDRDIRS = . ../include ../extern/ranges-v3/include

# OPTION: Change the compiler
#CXX = clang++

# Targets for building and running examples
%:%.cpp
	$(CXX) $(CXXFLAGS) $(foreach d,$(HDRDIRS),-I$(d)) $< -o $@
	./$@
