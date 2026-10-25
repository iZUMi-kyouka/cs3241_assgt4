# CS3241 Assignment 4

Repository: [iZUMi-kyouka/cs3241_assgt4](https://github.com/iZUMi-kyouka/cs3241_assgt4).

This repository bootstraps the supplied Bezier assignment. The assignment's unimplemented drawing tasks remain for you to complete.

## Build and format

Every `make` build runs `clang-format` first: four spaces, Egyptian braces (opening brace on the same line), and spaces around binary operators. Builds reject warnings by default. `make lint` checks formatting without editing files. CI uses clang-format 18.1.8, pinned in `requirements-dev.txt`.

```sh
make                         # Format, then build Debug
make CONFIG=Release          # Format, then build Release
make run                     # Format, build, then launch
make lint                    # Check formatting and original project integrity
make check                   # Check formatting and rebuild with zero warnings
make smoke                   # Also verify GLUT startup; requires a display
make clean
```

Install the formatter with `pipx install clang-format==18.1.8`, or `python -m pip install -r requirements-dev.txt` in an activated virtual environment. Python 3 is also required for the project-integrity and startup checks. `CLANG_FORMAT` and `PYTHON` can select other executable paths.

## Windows (priority)

Install **Visual Studio 2022**, the **Desktop development with C++** workload, **MSVC v143 x86/x64 tools**, and a **Windows 10 or 11 SDK**. Install GNU Make (`choco install make -y`), Python, and clang-format. Run from PowerShell, Command Prompt, or Git Bash with the tools on `PATH`:

```sh
make check CONFIG=Debug PLATFORM=x64
make check CONFIG=Release PLATFORM=x64
make check CONFIG=Debug PLATFORM=x86
make check CONFIG=Release PLATFORM=x86
make run PLATFORM=x64
```

Windows `make` locates Visual Studio 2022 with `vswhere` and rebuilds the **original `.sln` using MSBuild**. The solution maps `x86` to the project's `Win32`; either spelling is accepted by Make. Build outputs are under `build/x64/<configuration>/` or `build/Win32/<configuration>/`. The correct FreeGLUT DLL is copied beside the executable. Full compiler commands, text logs, and MSBuild binary logs are saved there.

You can also open `visual_studio/CS3241 Assignment 4 Bezier.sln` directly in Visual Studio and choose any of its four original configurations. Windows project files and dependencies live under `visual_studio/`; the shared source stays in the repository root. The project's source entry points to `..\main.cpp`. `Directory.Build.props` changes output locations; `Directory.Build.targets` supplies the missing include/library paths and copies the matching runtime. These files stay beside the solution.

### Professor's settings

Every compiler/linker setting in the supplied `.vcxproj` is **byte-for-byte unchanged**; its only edit is the source path needed by the move into `visual_studio/`. The `.sln` and original x64 binaries are completely unchanged. `scripts/verify-project.py` checks their original SHA-256 hashes before every Make build and in CI, normalizing only that single source-path edit. No Windows compiler or linker flag is overridden by the build wrapper.

| Setting | Original configuration retained |
| --- | --- |
| Toolset / SDK selection | v143 / `WindowsTargetPlatformVersion=10.0` |
| Character set | Unicode |
| Warnings | Level 3 (`/W3`) |
| Security / conformance | SDL checks (`/sdl`), conformance mode (`/permissive-`) |
| Language standard | Unspecified in project; MSVC's default C++14 |
| Debug | Debug libraries; `_DEBUG`, `_CONSOLE`; `WIN32` for Win32 |
| Release | Release libraries; `NDEBUG`, `_CONSOLE`; `WIN32` for Win32 |
| Release optimization | Original whole-program optimization, function-level linking, intrinsics, and inherited MSVC defaults |
| Linker | Console subsystem; debug information enabled in all configurations |

`make check` fails if the MSBuild log contains any compiler, linker, or MSBuild warning. It deliberately enforces this by inspecting the log, preserving `/W3` and the original warning treatment. Microsoft documents [warning levels](https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170), [SDL checks](https://learn.microsoft.com/en-us/cpp/build/reference/sdl-enable-additional-security-checks?view=msvc-170), and [conformance/default C++14](https://learn.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance?view=msvc-170).

The starter bundled only x64 FreeGLUT. Matching Win32 binaries were added for the original x86 configurations; see [dependency provenance](third_party/freeglut/README.md).

## macOS Intel and Apple Silicon

Install Apple's command line tools (`xcode-select --install`), Python 3, and the formatter. No Homebrew GLUT library is needed. Make uses the native architecture reported by `uname -m`, Apple Clang, and the system OpenGL and GLUT frameworks:

```sh
make check CXX=clang++
make check CONFIG=Release CXX=clang++
make run CXX=clang++
```

Run in a native terminal (outside Rosetta for Apple Silicon). These builds use C++14, `-Wall -Wextra -Wpedantic`, Debug `-O0 -g` or Release `-O2 -g`, and the corresponding debug/release macros. `make check` also makes compiler/linker warnings fatal. Clang cannot reproduce MSVC-only switches such as `/sdl`; the actual MSVC builds remain the grading compatibility check.

Apple deprecated its OpenGL/GLUT APIs. The Apple-only `GL_SILENCE_DEPRECATION` / `GLUT_SILENCE_DEPRECATION` macros acknowledge the assignment's required legacy APIs; other source warnings remain enabled. Windows warning settings are untouched.

## Linux

```sh
sudo apt-get install g++ make python3 freeglut3-dev libglu1-mesa-dev
pipx install clang-format==18.1.8
make check
make run
```

For a headless startup check, install `xvfb xauth` and run `xvfb-run -a make smoke`.

## GitHub checks and pushing

See [CI_SETUP.md](CI_SETUP.md) for the ten build/startup jobs, required checks on `main`, and the pre-push hook. Enable the hook in every new clone with `make install-hooks`.

Build checks verify compilation, linking, and five seconds of GLUT startup. They do not assess the unfinished assignment's curve behavior or visual correctness. Keep your eventual drawing data (`savefile.txt`) in version control for submission.
