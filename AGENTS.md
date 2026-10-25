# Project overview

This is `iZUMi-kyouka/cs3241_assgt4`, a CS3241 C++14/OpenGL/GLUT assignment about
Bezier curves. Windows grading compatibility is the first priority. Support native
Windows x64/x86, macOS Intel, macOS Apple Silicon, and Linux without compiler or
linker warnings.

Read `README.md` for setup, `CI_SETUP.md` for push checks, and
`cs3241_assgt4.pdf` before changing assignment behavior. The starter's drawing tasks
are not all implemented; a successful build or startup check does not establish
assignment correctness.

## Repository layout

- `main.cpp`: shared application source and GLUT callbacks; implement assignment
  behavior here unless the requested change needs another structure.
- `Makefile`: supported entry point for formatting, building, running, and checking.
- `.clang-format`: required C++ formatting rules.
- `requirements-dev.txt`: pinned clang-format version, currently 18.1.8.
- `scripts/format-source.py`: invokes the formatter without confusing CLion's
  Makefile compiler detection.
- `scripts/build-windows.ps1`: locates Visual Studio 2022, rebuilds the original
  solution with MSBuild, records diagnostics, and rejects warnings in strict mode.
- `scripts/verify-project.py`: checks the original project configuration and x64
  dependencies against their original SHA-256 hashes.
- `scripts/smoke-test.py`: launches the program, checks its startup banner and
  diagnostics, and verifies that it stays running for five seconds.
- `visual_studio/`: all Visual Studio project files, property/target files, bundled
  Windows headers, libraries, DLLs, and the professor's sample executable.
- `third_party/freeglut/`: dependency provenance and license.
- `.github/workflows/cross-platform-build.yml`: ten hosted build/startup jobs.
- `.githooks/pre-push`: validates outgoing commits before allowing the original push.
- `build/`: ignored generated executables and diagnostics; safe to remove with
  `make clean`.

Keep Windows-specific files and dependencies under `visual_studio/`. Keep shared
source in the repository root. Preserve the platform-specific include branches:
bundled FreeGLUT on Windows, Apple frameworks on macOS, system GLUT on Linux.

## Setup and commands

Run all commands from the repository root. Install GNU Make, Python 3, a native
C++ compiler, and clang-format 18.1.8. Install the formatter using
`pipx install clang-format==18.1.8`, or install `requirements-dev.txt` in an activated
Python virtual environment. `PYTHON` and `CLANG_FORMAT` select executable paths.

```sh
make                         # Format, verify configuration, build Debug
make CONFIG=Release          # Format, verify configuration, build Release
make run                     # Format, build, launch the interactive application
make format                  # Apply required formatting
make lint                    # Check formatting and project integrity without edits
make check                   # Check formatting, then rebuild with zero warnings
make check CONFIG=Release
make smoke                   # Run checks, then verify GLUT startup; needs a display
make clean                   # Remove generated build outputs
```

`CONFIG` accepts only `Debug` or `Release`; Debug is the default. Builds reject
warnings by default. Fix warnings in the source instead of disabling checks or
passing `STRICT=0`. Use the public Make targets; `_build` is an internal target
that bypasses formatting and integrity checks when invoked directly.

### Windows

Install Visual Studio **2022** with Desktop development with C++, MSVC **v143**
x86/x64 tools, and a Windows 10/11 SDK. GNU Make, Python, PowerShell, and the
formatter must be available. `vswhere` locates the VS2022 installation.

```sh
make check CONFIG=Debug PLATFORM=x64
make check CONFIG=Release PLATFORM=x64
make check CONFIG=Debug PLATFORM=x86
make check CONFIG=Release PLATFORM=x86
make run PLATFORM=x64
```

`PLATFORM` defaults to `x64`; `x86` and `Win32` both select the original Win32
project configuration. Outputs live under `build/x64/<configuration>/` or
`build/Win32/<configuration>/`, including `CS3241 Assignment 4 Bezier.exe`, its
matching `freeglut.dll`, `msbuild.log`, and `msbuild.binlog`. Never mix x64 and x86
FreeGLUT libraries or runtimes.

### macOS

Install Apple's command line tools (`xcode-select --install`), Python, and the
formatter. Use `make check CXX=clang++` and `make run CXX=clang++`; add
`CONFIG=Release` for Release. Link the system OpenGL and GLUT frameworks; do not
introduce a Homebrew GLUT dependency. Use a native terminal, outside Rosetta on
Apple Silicon. Outputs are `build/<uname -m>/<configuration>/cs3241_assgt4`.

### Linux

Install `g++ make python3 freeglut3-dev libglu1-mesa-dev`, plus the formatter.
Use `make check` and `make run`. For headless startup validation, install
`xvfb xauth` and run `xvfb-run -a make smoke`. Outputs use the same layout as macOS.

## Configuration invariants

The professor's `.vcxproj` and `.sln` under `visual_studio/` are the Windows build
authority. Windows Make builds must invoke that solution through MSBuild. Do not
replace them with separately reconstructed compiler commands or CMake settings.

- Preserve v143, SDK selection `10.0`, Unicode, `/W3`, `/sdl`, `/permissive-`, the
  original Debug/Release macros and runtime selection, Release optimization, and
  console/debug-information linker settings.
- The project leaves the language standard unspecified; its MSVC default is
  C++14. Keep shared code compatible with C++14. Unix builds explicitly use
  `-std=c++14`; they cannot reproduce MSVC-only switches.
- Preserve the original `.sln` and x64 binaries byte for byte. The `.vcxproj`'s
  only existing edit is `main.cpp` becoming `..\main.cpp` after relocation.
  Preserve its other bytes and CRLF line endings. `.gitattributes` protects them.
- `visual_studio/Directory.Build.props` controls output locations.
  `visual_studio/Directory.Build.targets` supplies dependency paths and copies the
  appropriate DLL. Neither should override professor-specified compiler flags.
- Windows strict checks inspect MSBuild logs for warnings, preserving the original
  warning treatment. Do not change `/W3` to `/W4` or add `/WX` as a substitute.
- Do not change verifier hashes, suppress diagnostics, replace bundled dependencies,
  or relax CI to conceal a build failure. Investigate and fix its cause.
- Retain the existing Apple-only legacy API deprecation macros; do not broaden
  warning suppression to Windows or unrelated source diagnostics.

## Formatting and CLion

Use four spaces, no indentation tabs in C++, Egyptian braces (opening brace on
the same line), and spaces around binary operators. `.clang-format` is authoritative;
Makefile recipes still require tabs. Format project source, not vendored headers.

Keep formatter calls behind `scripts/format-source.py`. A direct
`clang-format ... main.cpp` recipe causes CLion to mistake the formatter for a C++
compiler and probe it with flags such as `-xc++` and `-E`.

Open the root Makefile as a CLion project. Use GCC or Clang as the compiler and
configure clang-format separately under Editor > Code Style > C/C++. After build
configuration changes, use Tools > Makefile > Clean and Reload Makefile Project.
Keep machine-specific `.idea/` settings untracked.

## Validation and GitHub

For C++ changes, run `make format`, `make check`, and
`make check CONFIG=Release`. Add a startup check when startup, dependencies, or
build configuration changes. Verify rendering and input behavior interactively
when changing drawing tasks; the smoke test covers initialization only.
Documentation-only changes do not require local compilation.

CI tests Debug and Release on Windows x64, Windows x86, macOS x86_64, macOS arm64,
and Linux: ten jobs total. Keep that coverage and the Windows log checks intact.
Only report platforms as verified when their corresponding checks have passed.

For a new clone, install GitHub CLI, authenticate with `gh auth login`, and run
`make install-hooks` (use Git Bash for hook installation on Windows). A normal
`git push` submits the exact outgoing commits to temporary `ci-validation/*`
branches, waits for all jobs, removes those branches, and then completes the push.
`main` requires all ten checks and disallows force pushes. Do not bypass the hook
or protection to publish unchecked changes. See `CI_SETUP.md` for details.

## Assignment data

`make run` starts in the repository root, where the application reads and writes
`savefile.txt`. Preserve and version the user's drawing data; do not overwrite it
for smoke tests or commit generated binaries. Consult the assignment PDF for
submission filenames and drawing restrictions, including calculating curves
yourself and specifying object vertices/normals instead of using predefined object
drawing helpers.
