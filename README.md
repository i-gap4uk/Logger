# Logger Project

This project is a customizable, thread-safe logging library in C++. The logger supports multiple log destinations such as console, file, and more. Each `Logger` instance can be configured to log messages to specific destinations, and the library is designed for use in a multithreaded environment.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [Basic Setup](#basic-setup)
  - [Adding Custom Log Destinations](#adding-custom-log-destinations)
  - [Thread Safety](#thread-safety)

## Features

- **Multiple Log Destinations**: Log messages to various destinations.
- **Thread-Safe**: Safe to use in multithreaded applications.
- **Customizable**: Easily extendable to add new logging destinations.
- **Log levels**: Logger has several levels of logging, such as: ```INFO```, ```DEBUG```, ```WARNING``` and ```ERROR```.
- **Customizable Prefix**: Each Logger instance could have its own log prefix, that will be included into the each logline that this instance logs. 

## Prerequisites

- **C++11 or later**: Ensure that your environment supports at least C++11.

## Installation

### 1. Clone the Repository

Clone this repository to your local machine:

```bash
git clone https://github.com/i-gap4uk/multithreading_logger.git
```

### 2. Build the project
```bash
cd multithreading_logger
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

### Basic setup
You can create a Logger instance via several ways:

1. Without Prefix:
```cpp
auto logger = getLogger();
```

In this case, this instance of ```Logger``` will have no prefix in its loglines. Example:
```
01/01/1970 00:00:00; INFO; (128581333313344): Starting the app
```
Breakdown:

- ```01/01/1970 00:00:00``` - datetime stamp,
- ```INFO``` - log level (the ```INFO``` is default),
- ```(128581333313344)``` - a thread id, in which the instance is running,
- ```Starting the app``` - log itself.


2. Another Instance without Prefix:
```cpp
Logger logger2;
```
Similar to the first case, this instance will have no prefix in its log lines.

3. With a Custom Prefix:
```cpp
Logger logger3("instance3");
```
This instance of ```Logger``` will have prefix ```instance3``` in its loglines. Example:
```bash
01/01/1970 00:00:00; INFO; instance3(128581333313344): Starting the app
```

### Adding Custom Log Destinations
By default, the Logger logs into the console. There is also a predefined log destination for logging to a file (with a predefined non-configurable file path).
You can add your own custom ```LogPrinter``` via the ```add_log_printer method```:
```cpp
logger.add_log_printer(std::make_shared<CustomLogPrinter>());
```

### Thread Safety
The Logger is designed to be thread-safe. Each Logger instance manages its own message queue, and logging operations are synchronized internally. The static collection of printers is protected by a mutex to prevent race conditions when adding or removing printers dynamically.