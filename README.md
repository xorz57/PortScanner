<h1 align="center">PortScanner</h1>

```
Allowed options:
  --help                  display help message
  --host arg (=127.0.0.1) set host
  --protocol arg (=tcp)   set protocol (tcp/udp)
  --begin-port arg (=0)   set begin-port
  --end-port arg (=65535) set end-port
  --show arg (=all)       display only 'open', 'closed', or 'all' ports
```

## Example

```
./PortScanner --host uoc.gr --show open
Port 22/tcp is open.
Port 53/tcp is open.
Port 111/tcp is open.
Port 389/tcp is open.
Port 409/tcp is open.
Port 656/tcp is open.
Port 636/tcp is open.
Port 2000/tcp is open.
Port 3998/tcp is open.
Port 3999/tcp is open.
Port 5060/tcp is open.
Port 21162/tcp is open.
Port 21163/tcp is open.
Port 41314/tcp is open.
Port 43917/tcp is open.
Port 47603/tcp is open.
Port 52116/tcp is open.
```

## Dependencies

PortScanner relies on the following libraries:

- [asio](https://github.com/boostorg/asio)
- [program_options](https://github.com/boostorg/program_options)

## How to Build using CMake

```bash
cmake -B build
cmake --build build --config Release
ctest -C Release
```
