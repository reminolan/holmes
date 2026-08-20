
# Holmes

Holmes is an episodic adventure game developed by Remi Nolan, inspired by the classic Sherlock Holmes tales by Sir Arthur Conan Doyle.

This repository only contains the public source code, not the actual assets.

## Building

Holmes is built using CMake, Ninja, and a modern C compiler like GCC or Clang (not MSVC).

Holmes only depends on SDL3.
There are two ways to include the dependencies:

1. Compiled alongside the source code via git submodules (run `git submodules update --init --recursive`)
2. Install via system-wide package manager (e.g. `pacman -ySu SDL3-devel)

The CMake build script will first look for the dependencies under `<repo-root>/vendor/` where the git submodules will place them.
If it fails to find the dependencies there, it will attempt to use `find_package` to pull them from the system.
Otherwise the build configuration will fail.

Once the dependencies are available simply run the following commands from the repository root:

```sh
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=RelWidthDebugInfo
cmake --build build
```

This will output the executable at `<repo-root>/build/holmes[.exe]`.

> Note for macOS users: this does not package the `.app` bundle, it only compiles a UNIX executable.
> The App Bundle must be packaged manually, using the game assets extracted from a purchased copy of the game.

