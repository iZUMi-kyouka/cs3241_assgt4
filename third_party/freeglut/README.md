# FreeGLUT provenance

The assignment's `freeglut.dll`, `lib/freeglut.lib`, and `include/GL/*` are retained under `visual_studio/`. The runtime/import library are x64 FreeGLUT 3.0.0 from the Transmission Zero MSVC distribution. All repository paths in the table below are relative to `visual_studio/`.

The missing x86 runtime and import library were copied from a mirror of that same distribution, pinned at [dhustkoder/freeglut@451aa07d4c73418cdffb35c78ebcd9cf0e52845d](https://github.com/dhustkoder/freeglut/tree/451aa07d4c73418cdffb35c78ebcd9cf0e52845d). The mirror's x64 binaries have **identical SHA-256 hashes to the assignment's x64 binaries**, establishing that it is the matching package. Its license is included as `LICENSE.txt`.

| Repository file | Upstream path | SHA-256 |
| --- | --- | --- |
| `freeglut.dll` (original x64) | `bin/x64/freeglut.dll` | `c9dbe7459c26a8d03b14d22741e6a0e48e29541203ecbbcb578a01d02692d200` |
| `lib/freeglut.lib` (original x64) | `lib/x64/freeglut.lib` | `a778b0d13c99ecea9597f49f2bec044d9f60fe6e9d5671e6274e057adce84560` |
| `bin/Win32/freeglut.dll` (added x86) | `bin/freeglut.dll` | `b2d273ba52c2d842881c8584e9df94d17a25f96b9092b664ef3f275ce8a0d237` |
| `lib/Win32/freeglut.lib` (added x86) | `lib/freeglut.lib` | `6ed233488b42d8e0ab5a533eedae19a28fee51962c810e887c44779393a03bde` |

The original files are checked before builds. Windows selects the matching library and copies the corresponding runtime beside each executable. No network download is required at build time.
