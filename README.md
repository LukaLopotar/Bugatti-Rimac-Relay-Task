# Description
This repo is used for my relay controller task solution entry regarding the Bugatti-Rimac interview for the position of Embedded Software Engineer.

It is configured so that it can be opened in a Docker container with everything already set up so that it is possible to compile the code and run the simulation through the console without any manual installations. Using a Docker container is not necessary though and the code will work without it, although Docker container is convenient.

I've used GCC as my compiler of choice for this task and I've also set up Makefile so that compiling and running is as simple as running `make` and `make run` respectivelly (more on that later).

The code is written in C (C99 standard) with embedded design patterns in mind. Since it's not running a scheduler, some things are mocked to get the similar effect (more information further below).

# Folder structure

```
Bugatti-Rimac-Relay-Task/
├── .devcontainer/ -> Docker container specific files
│   ├── Dockerfile
│   └── devcontainer.json
├── .gitignore
├── Makefile -> All the targets that can be run with make
├── README.md
├── include/
│   ├── hal_dio.h -> Mock of HAL driver for DIO (Digital Input/Output)
│   ├── relay_constants.h -> Common constants used for relay definition
│   ├── relay_controller.h -> Main controller which monitors and processes requests for relays
│   ├── relays_config.h -> Configurations and settings for relays definition
│   └── simulation_config.h -> Configuration and settings for running mocks and simulations of the system
└── src/
    ├── hal_dio.c
    ├── main.c
    ├── relay_controller.c
    ├── relays_config.c
    └── simulation_config.c
```

# Building and running the software

This repo is using Makefile to make building and running the code a very simple process.

## Building

In order to build the software run the following command:
```bash
make
```
Note: If you want a clean build, you can run `make clean` command before running `make`.

## Running

In order to build the software run the following command:
```bash
make run
```
Note: The software is currently set up so that it will mock the HAL drivers and run an example simulation with hardcoded relay requests.

# Code structure

Conceptually, the code is structured in the following way:

## HAL DIO (Digital Input/Output)

The `hal_dio.h` and `hal_dio.c` files are used as a simple mock of setting the DPO level of a relay by using the `HAL_Dio_WriteDpo` function, and also reading the DI level of a relay by using the `HAL_Dio_ReadDi`.

## Relays configuration and settings

The `relays_config.h` and `relays_config.c` files are used to define all the options available for relay configuration and expose it to other parts of the system (relay controller for example).

The `relays_config.c` file consist of a configuration table for all the available relays. You can easily add, modify or remove relays without impacting/breaking other parts of the code.

## Relay controller

The `relay_controller.h` and `relay_controller.c` files are defining the relay controller. The main public APIs for the relay controller are as follows:
* `RelayController_Init` - to initialize the controller and set all the relays to their starting values.
* `RelayController_MonitorRelaysTask` - a periodic function which monitors the states of all relays and checks if relays have opened/closed successfully after feedback stabilization time of 30 ms. In a RTOS, this function would be called as a task.
* `RelayController_SetRequest` - a function used to change DPO level of a relay from anywhere in the system.
* `RelayController_GetRelayState` - a function to fetch the current state of a particual relay (is it open, closed or in error etc.)
* `RelayController_GetRelayErrorType` - a function to fetch the error type of a relay (welded or constantly open), if the relay is in error state.

The relay controller also has its own internal state. In the case that one of the relays is in error, the relay controller also goes into its own error state and does the following:
* Opens all of the relays
* Doesn't allow processing of any new requests to change the DPO level of a relay

## Simulation configuration

Although not part of the task solution, there are `simulation_config.h` and `simulation_config.c` files which help in setting up and running the mock up version of the system.
