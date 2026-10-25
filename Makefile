.DEFAULT_GOAL := all

CONFIG ?= Debug
PYTHON ?= python3
CLANG_FORMAT ?= clang-format
STRICT ?= 1

ifeq ($(filter $(CONFIG),Debug Release),)
$(error CONFIG must be Debug or Release)
endif

ifeq ($(OS),Windows_NT)
PLATFORM ?= x64
PYTHON = python
POWERSHELL ?= powershell.exe
WINDOWS_BUILD = $(POWERSHELL) -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -Configuration $(CONFIG) -Platform $(PLATFORM)
EXECUTABLE = build/$(if $(filter x86 Win32,$(PLATFORM)),Win32,x64)/$(CONFIG)/CS3241 Assignment 4 Bezier.exe
else
HOST_OS := $(shell uname -s)
PLATFORM ?= $(shell uname -m)
EXECUTABLE = build/$(PLATFORM)/$(CONFIG)/cs3241_assgt4
PROJECT_FLAGS = -std=c++14 -Wall -Wextra -Wpedantic
ifeq ($(CONFIG),Debug)
PROJECT_FLAGS += -O0 -g -D_DEBUG -D_CONSOLE
else
PROJECT_FLAGS += -O2 -g -DNDEBUG -D_CONSOLE -ffunction-sections -finline-functions
endif
ifeq ($(STRICT),1)
PROJECT_FLAGS += -Werror
endif
ifeq ($(HOST_OS),Darwin)
PROJECT_LIBS = -framework OpenGL -framework GLUT
ifeq ($(STRICT),1)
PROJECT_LINK_FLAGS = -Wl,-fatal_warnings
endif
else
PROJECT_LIBS = -lglut -lGLU -lGL
ifeq ($(STRICT),1)
PROJECT_LINK_FLAGS = -Wl,--fatal-warnings
endif
endif
endif

.PHONY: all build _build format format-check lint verify-project strict check run smoke clean help install-hooks

all: build

# Recursion ensures Make sees source timestamps after the formatter has finished.
build: format verify-project
	$(MAKE) --no-print-directory _build

format:
	$(PYTHON) scripts/format-source.py --formatter "$(CLANG_FORMAT)"

format-check:
	$(PYTHON) scripts/format-source.py --formatter "$(CLANG_FORMAT)" --check

lint: format-check verify-project

verify-project:
	$(PYTHON) scripts/verify-project.py

strict: format verify-project
	$(MAKE) --no-print-directory _build STRICT=1

# CI checks formatting before any build can automatically fix it.
check: lint
	$(MAKE) --no-print-directory strict

ifeq ($(OS),Windows_NT)
_build:
	$(WINDOWS_BUILD) $(if $(filter 1,$(STRICT)),-Strict,)

clean:
	$(POWERSHELL) -NoProfile -Command "if (Test-Path build) { Remove-Item -Recurse -Force build }"
else
_build:
	mkdir -p "$(dir $(EXECUTABLE))"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(PROJECT_FLAGS) main.cpp $(LDFLAGS) $(PROJECT_LINK_FLAGS) $(PROJECT_LIBS) -o "$(EXECUTABLE)"

clean:
	rm -rf build
endif

run: build
	"$(EXECUTABLE)"

smoke: check
	$(PYTHON) scripts/smoke-test.py "$(EXECUTABLE)"

install-hooks:
	bash scripts/install-pre-push-hook.sh

help:
	@echo "make [CONFIG=Debug or Release] [PLATFORM=x64 or x86 on Windows]"
	@echo "Targets: build (formats first), run, format, lint, check, smoke, clean, install-hooks"
	@echo "Windows builds the original VS2022 solution; macOS/Linux use native C++14."
