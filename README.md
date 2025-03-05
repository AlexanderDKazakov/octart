# Octart

CLI app for file transfer. The app is both client and server

##
```bash
cmake -B build && cmake --build build
cmake -B build && cmake --build build --target tests
```

## Usage
### Server
```bash
./octart -s # `s` states for server
```

### Client
```bash
./octart -c path_to_file # `c` states for client
```

## Features:

* Files are saved on the server with a prefix indicating the time of receipt.
* TCPServer is waiting for the client/s (saves files within its location date_time.hex)
* Client send file and terminates
* Notification of sending/receiving.


### Bonus
* CRC32 verification ensures the integrity of the transmission. [WIP]
