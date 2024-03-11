all: chessplusplus

PROJECT_NAME = chessplusplus
VERSION_MAJOR = 1
VERSION_MINOR = 2
VERSION_PATCH = 0
VERSION = "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)"

COMP ?= gcc
COMPXX ?= g++

MAKE ?= make


chessplusplus: 
	@+$(MAKE) -C engine