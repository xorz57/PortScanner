# PortScanner

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Usage

```bash
Allowed options:
  --help                  display help message
  --host arg (=127.0.0.1) set host
  --port arg (=0-65535)   set port range in the format 'begin[-end]'
  --protocol arg (=tcp)   set protocol (tcp/udp)
  --show arg (=open)      display only 'open', 'closed', or 'all' ports
```

## Example

```bash
./PortScanner --host boost.org
Port 25/tcp is open.
Port 22/tcp is open.
Port 80/tcp is open.
Port 443/tcp is open.
Port 587/tcp is open.
Port 8080/tcp is open.
```

## Dependencies

PortScanner relies on the following libraries:

- [asio](https://github.com/boostorg/asio)
- [program_options](https://github.com/boostorg/program_options)

## How to Build

#### Linux & macOS

```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh

git clone https://github.com/xorz57/PortScanner.git
cd PortScanner
cmake -B build -DCMAKE_BUILD_TYPE=Release -S . -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
ctest --build-config Release
```

#### Windows

```powershell
git clone https://github.com/microsoft/vcpkg.git C:/vcpkg
C:/vcpkg/bootstrap-vcpkg.bat
C:/vcpkg/vcpkg.exe integrate install

git clone https://github.com/xorz57/PortScanner.git
cd PortScanner
cmake -B build -DCMAKE_BUILD_TYPE=Release -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
ctest --build-config Release
```
