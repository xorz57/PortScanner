# PortScanner

Port Scanner written in C++20

## Usage

```
PortScanner --host <host> --begin-port <begin-port> --end-port <end-port> [--show open/closed/all]
```

## How to Build using CMake

```bash
cmake -B build
cmake --build build --config Release
ctest -C Release
```