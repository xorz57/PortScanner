<h1 align="center">PortScanner</h1>

```bash
Allowed options:
  --help                  display help message
  --host arg (=127.0.0.1) set host
  --begin-port arg (=0)   set begin-port
  --end-port arg (=65535) set end-port
  --show arg (=all)       display only 'open', 'closed', or 'all' ports
```

## Example

```bash
./PortScanner --host uoc.gr --show open
Port 22 is open.
Port 53 is open.
Port 111 is open.
Port 389 is open.
Port 409 is open.
Port 656 is open.
Port 636 is open.
Port 2000 is open.
Port 3998 is open.
Port 3999 is open.
Port 5060 is open.
Port 21162 is open.
Port 21163 is open.
Port 41314 is open.
Port 43917 is open.
Port 47603 is open.
Port 52116 is open.
```

## How to Build using CMake

```bash
cmake -B build
cmake --build build --config Release
ctest -C Release
```