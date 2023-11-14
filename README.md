# PortScanner

Port Scanner written in C++20

```
Allowed options:
  --help                  display help message
  --host arg (=127.0.0.1) set host
  --begin-port arg (=0)   set begin-port
  --end-port arg (=65535) set end-port
  --show arg (=all)       display only 'open', 'closed', or 'all' ports
```

## How to Build using CMake

```bash
cmake -B build
cmake --build build --config Release
ctest -C Release
```