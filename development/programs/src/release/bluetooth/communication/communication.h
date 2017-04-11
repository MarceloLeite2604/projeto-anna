/*
 * This header file contains all components required to control the bluetooh communication.
 *
 * Version: 0.1
 * Author: Marcelo Leite
 */
#ifndef BLUETOOTH_COMMUNICATION_H
#define BLUETOOTH_COMMUNICATION_H

/*
 * Includes.
 */
#include "../package/package.h"
#include "../connection/connection.h"

/*
 * Function headers.
 */

/* Checks if a device is connected. */
int check_connection(int);

/* Checks if a device is connected. */
/* int is_connected(int); */

/* Receives a package from a connection. */
int receive_package(int, package_t*);

/* Sends a file through a connection. */
int send_file(int, char*);

/* Transmits a command result. */
int transmit_command_result(int, int);

/* Transmits an error. */
int transmit_error(int, int, const char*);

#endif