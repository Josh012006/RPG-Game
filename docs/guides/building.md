# Building

> [Docs index](../README.md) · [Architecture](../ARCHITECTURE.md#22-build-targets) · Related: [Testing](testing.md) · [Engine](../features/engine.md#start-up)

## Requirements

| Need | Version |
|---|---|
| C++ compiler with C++17 | GCC 8+, Clang 7+, MSVC 2019+ |
| CMake | 3.16+ |
| SFML | **2.6.x** (`graphics`, `window`, `system`). **Not SFML 3**: its API is incompatible |
| nlohmann/json | 3.2+ (a copy is bundled in `third_party/include/nlohmann/`) |

The tests and the `core` library need only the compiler and nlohmann/json (no SFML).

## Commands

```bash
# configure (once, or after changing CMakeLists.txt)
cmake -S . -B build

# build everything
cmake --build build --config Release

# run the tests
ctest --test-dir build -C Release --output-on-failure

# run the game (from the repository root)
./bin/oathbound            # Windows: .\bin\oathbound.exe
```

Always pass `--config Release` (or `Debug`) with Visual Studio generators: they are multi-configuration, and the default
is Debug.

### CMake options

| Option | Default | Effect |
|---|---|---|
| `OATHBOUND_BUILD_GAME` | `ON` | Build the SFML game. `OFF` builds only `core` and the tests (no SFML needed) |
| `OATHBOUND_BUILD_TESTS` | `ON` | Build `oathbound_tests` |
| `CMAKE_BUILD_TYPE` | `Release` | Single-configuration generators only |

```bash
cmake -S . -B build -DOATHBOUND_BUILD_GAME=OFF     # core + tests only
```

Targets are described in [Architecture](../ARCHITECTURE.md#22-build-targets). Outputs go to `bin/`.

## Platform notes

### Windows (bundled SFML)

CMake switches to the bundled SFML when `lib/` exists.

```text
third_party/include/SFML/...      SFML headers
third_party/include/nlohmann/     nlohmann headers
lib/sfml-*.lib                    import libraries (Release)
bin/sfml-*-2.dll                  DLLs, next to the executable
media/                            assets
```

The bundled libraries are **Release** libraries (`sfml-graphics.lib`, no `-d`): build in Release, or provide Debug libraries.
A missing DLL fails at start-up, before `main`.

### Linux / macOS

Install SFML 2.6 with the package manager and let `find_package(SFML 2.6 ...)` find it
(for example `libsfml-dev` on Ubuntu 24.04). Check that the package is version 2.x: some distributions now ship 3.x.

## Running

The game uses relative paths (`data/`, `media/`, `saves/`). `main.cpp` looks for the folder containing `data/items.json` in the
current directory, then around the executable, and switches to it, so it starts correctly from the root, from `bin/`, or by double-click.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `MSB8013 ... Debug\|x64` | The build tree was configured with different configurations than the one requested | Add `--config Release`; if it persists, delete `build/` and reconfigure |
| `LNK2019` / `LNK2001` / `undefined reference` | A **definition** is missing or duplicated: an old `.cpp` left in a globbed folder, a file not part of the build, a missing `target_link_libraries` | Delete leftovers from `src/engine/`, `src/states/`, `src/core/` (CMake reports `GLOB mismatch! ... removed`), rebuild |
| Old class names (`Josh::...`) in linker errors | Files from the prototype still in `src/` | See [Prototype migration](prototype-migration.md#upgrading-an-existing-working-copy) |
| `Could not find SFML` | SFML not installed or wrong version | Install SFML 2.6, or provide the bundled layout above |
| Game exits at launch, DLL error | SFML DLLs not next to the executable | Copy them into `bin/` |
| `Fatal error: ...items.json: ...` | Invalid data file | The message names the file and the entry; see [Data files](../reference/data-files.md) |
| `cannot load texture '...'` | Missing or misnamed asset | Check the path in `engine/Config.hpp` and the `media/` folder |
| Tests fail after editing `data/` | Some tests rely on the shipped data | See [Testing: tests that read real data](testing.md#tests-that-read-real-data) |

## Debugging memory problems

Build the core and tests with the sanitizers:

```bash
g++ -std=c++17 -g -O1 -fsanitize=address,undefined -Isrc -Ithird_party/include -Itests \
    -DOATHBOUND_DATA_DIR='"'$PWD'/data"' -DOATHBOUND_FIXTURES_DIR='"'$PWD'/tests/fixtures"' \
    $(find src/core -name '*.cpp') tests/*.cpp -o oathbound_tests_asan && ./oathbound_tests_asan
```

---

## Related

[Docs index](../README.md) · [Architecture](../ARCHITECTURE.md) · [Testing](testing.md) · [Engine](../features/engine.md) · [Prototype migration](prototype-migration.md)
