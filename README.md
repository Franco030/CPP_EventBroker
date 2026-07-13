# C++ Event Broker

A lightweight, concurrent, and persistent publish/subscribe event broker written in C++20. 

I built this project to deepen my understanding of distributed systems and networking in C++, while also putting my custom [C++ ORM (CPP_ORM)](https://github.com/Franco030/CPP_ORM) to the test for data persistence.

## Features

- **Real-Time Pub/Sub**: Supports multiple concurrent publishers and subscribers broadcasting over raw TCP sockets.
- **Durability**: Every published event is instantly saved to an embedded SQLite database using my custom C++ ORM, guaranteeing zero message loss.
- **High Concurrency**: Automatically spawns detached threads for each incoming client connection.
- **JSON Protocol**: Lightweight communication using JSON payloads (`nlohmann/json`).
- **No Heavy Frameworks**: Built using standard POSIX sockets (`<sys/socket.h>`) without relying on massive external networking libraries.

## Architecture

The project contains two main tables managed by the ORM:
- `Topic`: Represents a channel that clients can subscribe to.
- `Event`: Represents the actual message payload, foreign-keyed to a specific topic.

When a publisher sends an event, the broker:
1. Parses the JSON.
2. Saves it permanently to `broker.db` using the ORM.
3. Broadcasts the JSON to all connected clients subscribed to that specific topic.

## Build Instructions

### Prerequisites
- A C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.15 or higher

*(Dependencies like my `CPP_ORM` and `nlohmann/json` are automatically fetched by CMake!)*

### Building

```bash
git clone <your-repo-url>
cd cpp_event_broker
mkdir build && cd build
cmake ..
make
```

This will produce two executables in the `build` directory:
- `event_broker`: The main server daemon.
- `broker_client`: A handy CLI client for testing publishes and subscriptions.

## Usage

### 1. Start the Server
First, run the broker server. It will automatically create the `broker.db` SQLite database, initialize the tables, and bind to port `8080`.

```bash
cd build
./event_broker
```

### 2. Subscribe to a Topic
Open a new terminal and act as a subscriber. You can use the provided C++ client:

```bash
cd build
./broker_client subscribe
```

Alternatively, you can use `netcat` (`nc`):
```bash
echo '{"type": "subscribe", "topic_id": 0}' | nc localhost 8080
```
*The terminal will hang as it actively listens for incoming events.*

### 3. Publish an Event
Open a third terminal and send a payload to the topic:

```bash
cd build
./broker_client publish "Hello, subscribers!"
```

Or using `netcat`:
```bash
echo '{"type": "publish", "topic_id": 0, "payload": "Hello, subscribers!"}' | nc localhost 8080
```

You should instantly see the message appear in your Subscriber terminal, and if you inspect `broker.db`, you'll see the event safely persisted in the database!
