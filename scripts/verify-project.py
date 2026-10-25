"""Guard the professor's configuration, allowing only the relocated source path."""

import hashlib
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
ORIGINALS = {
    "visual_studio/CS3241 Assignment 4 Bezier.vcxproj":
        "67ae165277dcd70fb56dce2400eed84d1397aa78dd741e0942bc948236f21ed1",
    "visual_studio/CS3241 Assignment 4 Bezier.sln":
        "a2ca557341e9fd1257fefcee58993f11c579beb7c92669fb0aea8c340c0fa7af",
    "visual_studio/lib/freeglut.lib":
        "a778b0d13c99ecea9597f49f2bec044d9f60fe6e9d5671e6274e057adce84560",
    "visual_studio/freeglut.dll":
        "c9dbe7459c26a8d03b14d22741e6a0e48e29541203ecbbcb578a01d02692d200",
}

for name, expected in ORIGINALS.items():
    content = (ROOT / name).read_bytes()
    if name.endswith(".vcxproj"):
        # Moving the project one directory down requires this one source-path edit.
        # Every other byte, including all compiler/linker settings, must match.
        content = content.replace(
            b'ClCompile Include="..\\main.cpp"', b'ClCompile Include="main.cpp"'
        )
    actual = hashlib.sha256(content).hexdigest()
    if actual != expected:
        raise SystemExit(f"Original professor-supplied file was changed: {name}")

print("Professor's compiler/linker configuration, solution, and x64 binaries are unchanged.")
