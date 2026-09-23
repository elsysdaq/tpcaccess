# TpcAccess

TpcAccess is the client library for connecting applications to an Elsys device server over a network.

The public C API is documented in [`include/TpcAccess.h`](include/TpcAccess.h). Example applications are available under [`examples/tpcaccess-examples`](examples/tpcaccess-examples).

## Project structure

```text
include/                         Public C API
src/tpcaccess/                   Native library implementation
src/common/                      Shared native support code
src/wrappers/                    Python and .NET bindings
examples/tpcaccess-examples/     C++, C#, VB, and Python examples
examples/tpcaccess-prebuilt/     Prebuilt Windows x64 native library
extern/                          External dependencies
cmake/ and build/                Build infrastructure
```

## Build the native library

TpcAccess requires CMake 3.25 or newer and a C++20-capable compiler. Its third-party dependencies are included.

```sh
cmake -S . -B build -DBUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target tpcaccess
```

Use the `tpcaccess_static` target to build the static library. To build the included examples, omit `-DBUILD_EXAMPLES=OFF`.

## Examples

Before running an example, change its server address to the address of your TransPC server. The default TCP port is `10010`.

### Visual Studio

On Windows, open [`examples/tpcaccess-examples/vs-examples.sln`](examples/tpcaccess-examples/vs-examples.sln) in Visual Studio, select an `x64` configuration, and build or run the desired project. The solution contains C++, C#, and Visual Basic examples and uses the bundled Windows x64 TpcAccess DLL.

### CMake

The C++ examples can be built together with TpcAccess:

```sh
cmake -S . -B build -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The available example targets are `cont-example`, `ecr-example`, and `scope-example`. To build only one example, append `--target <name>` to the second command.

## Python package

From this directory, build and install the package and its native extension with:

```sh
python -m pip install .
```

The module can then be imported as follows:

```python
import tpcaccess as tpc
```

To create a wheel file without installing it:

```sh
python -m pip wheel . --wheel-dir dist
```

## License

TpcAccess is distributed under the MIT License. See [`LICENSE`](LICENSE).

## Version History

### [1.5.1] — 2026-09-23

#### Added
- Introduce CMake-based build for C++ library
- `pyproject.toml` — the python wrapper can now be built as a wheel and installed directly from the repository with `pip install .`
- Add TPC_GetParameterAvailableValues() API call

#### Changed
- Removed legacy Makefiles and VS-solution for tpcaccess library builds
- Overhauled project folder structure
- Integrate examples with CMake build and with the VS-solution example

#### Fixed
- Fixed memory leaks and data races

---

### [1.4] — 2024-02-22

#### Added
- Add build instructions and precompiled Python bindings to TpcAccess library
- Add support for strain measurement settings

---

### [1.3] — 2021-04-09

- Initial open source release
