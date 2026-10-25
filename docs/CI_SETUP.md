# Cross-platform push checks

The workflow in `../.github/workflows/cross-platform-build.yml` runs on `main`, temporary `ci-validation/*` branches, pull requests, and manual dispatch. Its ten required jobs are:

- `Windows x64 Debug`
- `Windows x64 Release`
- `Windows x86 Debug`
- `Windows x86 Release`
- `macOS x86_64 Debug`
- `macOS x86_64 Release`
- `macOS arm64 Debug`
- `macOS arm64 Release`
- `Linux Debug`
- `Linux Release`

Windows uses the Windows Server 2022 / Visual Studio 2022 image, rebuilding the actual supplied solution with the original v143 settings. It checks the full MSBuild log for warnings and uploads the executable, matching DLL, text log, and binary log. macOS uses explicit `macos-15-intel` (Intel) and `macos-15` (Apple Silicon) runners and verifies their architectures. These labels follow GitHub's [runner reference](https://docs.github.com/en/actions/reference/runners/github-hosted-runners).

Each job checks formatting, validates the original project/binaries, builds, and launches the GLUT program for five seconds to catch missing DLLs, missing frameworks, and startup crashes. Linux supplies a virtual display with Xvfb. Startup checks do not verify the drawing tasks.

## Local pre-push hook

Install GitHub CLI and Git, authenticate using `gh auth login`, then run:

```sh
make install-hooks
```

Use Git Bash for installation on Windows. The hook checks each distinct outgoing commit by pushing it to a temporary `ci-validation/*` branch, waiting for the ten jobs, and then continuing the original push only after success. It deletes temporary validation branches afterward. Uncommitted changes are not included. Set `GH_REPO=iZUMi-kyouka/cs3241_assgt4` if repository inference is unavailable.

The `Cross-platform checks on main` ruleset requires all ten checks and disallows force pushes. Direct pushes are allowed once checks have passed for that exact commit. There are no bypass actors. A skipped local hook therefore does not allow unchecked changes to `main`.
