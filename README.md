# Sensor Gateway

## Description

**Sensor Gateway** is a multi-process, multi-threaded system for collecting, processing, and storing temperature sensor data from multiple sensor nodes over TCP. The system supports event logging, SQLite storage, and is designed for extensibility, security, and high performance.

## Architecture

- **Sensor nodes**: Send temperature data via TCP.
- **Sensor gateway**:
  - **Main process**: Manages threads:
    - **Connection Manager**: Listens for and handles multiple concurrent sensor connections.
    - **Data Manager**: Computes running averages, detects hot/cold events, and logs alerts.
    - **Storage Manager**: Stores sensor data in SQLite and recovers from DB failures.
  - **Log process**: Receives log events via FIFO and writes them to a log file in a standard format.

## Features

- Handles multiple concurrent TCP connections.
- Event logging (connections, alerts, errors, DB events, etc.).
- Stores sensor data in SQLite.
- Thread-safe, no memory leaks.
- Easily extensible for authentication, encryption, CLI control, statistics, and more.

## Requirements

- Linux (tested on Ubuntu/Fedora)
- GCC, Make
- SQLite development library (`libsqlite3-dev` or `sqlite-devel`)

## Build & Run Instructions

### 1. Install SQLite (if not already installed)

**Ubuntu/Debian:**

```sh
sudo apt-get install sqlite3 libsqlite3-dev
```

**Fedora:**

```sh
sudo dnf install sqlite sqlite-devel
```

### 2. Build the project

```sh
make clean
make
```

### 3. Run the program

```sh
./main <port>
# Example:
./main 12345
```

### 4. Simulate sensor data

In another terminal:

```sh
echo "1 30.0 101 17800000" | nc localhost 12345
```

Format: `sensor_id temperature room_id timestamp`

### 5. Check logs and database

- Event log: `logs/gateway.log`
- Sensor data: `db/sensordata.db`
  ```sh
  sqlite3 db/sensordata.db "SELECT * FROM sensor_data;"
  ```

## Directory Structure

```
sensor-gateway/
├── src/                # C source code
├── inc/                # Header files
├── db/                 # SQLite database files
├── logs/               # Log files, FIFO
├── Makefile
├── README.md
```

## Extension Suggestions

- Multiplexing (epoll/select) for thousands of connections.
- Sensor authentication, TLS/SSL encryption.
- CLI control: status, stats, exit.
- Connection limits, DoS/DDoS protection.
- Memory optimization, leak checking with Valgrind.
- Modular code for easy maintenance.
