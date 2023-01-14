# Project Name
TARGET = oscillator

# Sources
# CPP_SOURCES = oscillator.cpp
CPP_SOURCES = $(wildcard *.cpp)

# Library Locations
LIBDAISY_DIR ?= ../../../libDaisy
DAISYSP_DIR ?= ../../../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

