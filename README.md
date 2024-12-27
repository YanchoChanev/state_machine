# FreeRTOS Posix GCC Project

This project demonstrates the integration of FreeRTOS in a POSIX-compatible environment using GCC.

## Table of Contents
- [Overview](#overview)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Setup](#setup)
- [Building the Project](#building-the-project)
- [Running Tests](#running-tests)
- [Naming Convention](#naming-convention)
- [Unit Tests](#unit-tests)
- [Architecture](#architecture)
- [Cleaning Up](#cleaning-up)
- [License](#license)

---

## Overview
This project uses FreeRTOS to provide real-time operating system capabilities within a POSIX-compliant environment. The application consists of master and slave components, along with logging functionality.

## Project Structure
```
├── include/       # Project-wide headers
├── master/        # Master-specific source and headers
│   ├── src/       # Source files
│   ├── include/   # Header files
│   ├── tests/     # Master-specific tests
├── slave/         # Slave-specific source and headers
│   ├── src/       # Source files
│   ├── include/   # Header files
│   ├── tests/     # Slave-specific tests
├── types/         # Type definitions
├── config/        # Configuration files
├── logger/        # Logging system
│   ├── src/       # Source files
│   ├── include/   # Header files
├── test_scripts/  # Test scripts
├── main.c         # Main application entry point
├── Makefile       # Build configuration
└── freertos_setup.sh # FreeRTOS setup script
```

## Dependencies
- GCC Compiler
- POSIX-compliant system
- pthread library
- FreeRTOS source code

Ensure that the `FREERTOS_PATH` environment variable is set to the root directory of FreeRTOS.

## Setup
Run the setup script to initialize the environment:
```bash
make setup
```

## Building the Project
To build the project:
```bash
make
```
The output binary will be located in the `build/` directory as `modelo-posix-gcc`.

## Running Tests
The project includes a set of predefined tests:

### Master Tests
- `make run_master_comm_test`
- `make run_master_handler_test`
- `make run_master_state_mashine_test`

### Slave Tests
- `make run_slave_comm_test`
- `make run_slave_handler_test`
- `make run_slave_restart_threads_test`
- `make run_slave_state_machine_test`

Each command executes the corresponding test script located in `test_scripts/`.

## Naming Convention
- **Directories:** Use lowercase letters with underscores (e.g., `master_src`, `slave_handler`).
- **Files:** Use descriptive names for source and header files (e.g., `master_handler.c`, `logger_utils.c`).
- **Variables:** Follow snake_case for variables and macro names.
- **Functions:** Use camelCase for function names, with a prefix indicating the module (e.g., `master_handleEvent`).
- **Constants:** Use uppercase with underscores (e.g., `MAX_BUFFER_SIZE`).

Consistent naming improves code readability and collaboration.

## Unit Tests
Unit tests are implemented using custom test scripts located in the `test_scripts/` directory.

### Running Unit Tests
To run specific unit tests:
```bash
make run_master_comm_test
make run_master_handler_test
make run_master_state_mashine_test
make run_slave_comm_test
make run_slave_handler_test
make run_slave_restart_threads_test
make run_slave_state_machine_test
```

### Adding New Unit Tests
1. Add a new test script in `test_scripts/`.
2. Update the `Makefile` with a new target for the test.
3. Follow the naming convention: `run_<module>_test.sh`.

Unit tests focus on validating individual functions and components to ensure reliability.

## Architecture
The detailed architecture diagram and explanation can be found [here](https://docs.google.com/document/d/15yoyWX8DCxcP7g0IB26MCoQuK6t1syV5jzvsIHMFAbM/edit?tab=t.0).

This document outlines the relationships and interactions between master, slave, logger, and other key components.

## Cleaning Up
To clean the build artifacts:
```bash
make clean
```

## License
This project follows the FreeRTOS licensing model. Refer to the [FreeRTOS License](https://www.freertos.org/a00114.html) for more details.

---

For further details, refer to the official FreeRTOS documentation: [FreeRTOS.org](https://freertos.org/Creating-a-new-FreeRTOS-project.html)

---
**Author:** Yancho Chanev
**Version:** 1.0.0
