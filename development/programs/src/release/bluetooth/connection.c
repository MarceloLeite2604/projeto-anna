/*
 * This source file contains the elaboration of all components required to manipulate a bluetooth connection.
 *
 * Version: 
 *  0.1
 *
 * Author: 
 *  Marcelo Leite
 */

/*
 * Includes.
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include "bluetooth/package/codes.h"
#include "bluetooth/connection.h"
#include "log.h"
#include "return_codes.h"


/*
 * Macros.
 */

/* Size of the buffer to read content from the socket. */
#define READ_CONTENT_BUFFER_SIZE 1024

/* Error code received through "errno" variable when the connection was reseted by the peer. */
#define ERROR_CODE_CONNECTION_RESET_BY_PEER 104


/*
 * Variables.
 */

/* Time to wait for a content to be read on socket. */
const struct timeval _read_wait_time = { .tv_sec = 0, .tv_usec = 0 };


/*
 * Function elaborations.
 */

/*
 * Closes a socket communication.
 *
 * Parameters
 *  socket_fd - The file descriptor of the socket communication to be closed.
 *
 * Returns
 *  SUCCESS - If the communication was closed successfully.
 *  GENERIC ERROR - Otherwise.
 */
int close_socket(int socket_fd){
    LOG_TRACE_POINT;

    int result;
    int close_result;

    close_result = close(socket_fd);
    if (close_result < 0 ) {
        LOG_ERROR("Error while closing socket.");
        LOG_ERROR("%s", strerror(errno));

        result = GENERIC_ERROR;
    }
    else {
        LOG_TRACE_POINT;

        result = SUCCESS;
    }

    LOG_TRACE_POINT;
    return result;
}

/*
 * Checks if there is content to read on a socket.
 *
 * Parameters
 *  socket_fd - The socket communication file descriptor to be checked.
 *  check_time - Time to wait for a content to be avilable on socket.
 *
 * Returns
 *  NO_CONTENT_TO_READ - If there is no content to be read.
 *  CONTENT_TO_READ - If the socket has content to read.
 *  GENERIC_ERROR - If there was an error checking the socket content.
 */
int check_socket_content(int socket_fd, struct timeval check_time) {
    LOG_TRACE_POINT;

    int result;
    int select_result;
    fd_set socket_fd_set;

    FD_ZERO(&socket_fd_set);
    FD_SET(socket_fd, &socket_fd_set);
    select_result = select((socket_fd+1), &socket_fd_set, (fd_set*)NULL, (fd_set*)NULL, &check_time);
    FD_CLR(socket_fd, &socket_fd_set);
    LOG_TRACE("Select result: %d.", select_result);

    switch (select_result) {
        case 0:
            LOG_TRACE("No content on socket.");
            result = NO_CONTENT_TO_READ;
            break;

        case -1:
            LOG_ERROR("Error while checking socket.");
            result = GENERIC_ERROR;
            break;

        default:
            LOG_TRACE("Found content on socket.");
            result = CONTENT_TO_READ;
            break;
    }

    LOG_TRACE_POINT;
    return result;
}

/*
 * Reads content from a socket.
 *
 * Parameters
 *  socket_fd - The socket communication file descriptor to read content.
 *  byte_array - The structure to store content read from socket.
 *
 * Returns
 *  SUCCESS - If a content was successfully read from socket.
 *  NO_CONTENT_TO_READ - If there was no content to read from socket.
 *  DEVICE_DISCONNECTED - When the connection was lost.
 *  GENERIC_ERROR - If there was an error reading the socket.
 */
int read_socket_content(int socket_fd, byte_array_t* byte_array) {
    LOG_TRACE("Socket file descriptor: %d", socket_fd);

    int result;
    struct timeval read_wait_time = _read_wait_time;
    int select_result;
    uint8_t buffer[READ_CONTENT_BUFFER_SIZE];
    uint32_t* array_pointer = (uint32_t*)buffer;
    ssize_t content_size = 0;
    ssize_t total_read;
    bool error = false;
    bool done_reading = false;
    int errno_value;
    int copy_content_result;

    delete_byte_array(byte_array);
    LOG_TRACE_POINT;

    while (done_reading == false ) {
        LOG_TRACE_POINT;

        select_result = check_socket_content(socket_fd, read_wait_time);
        LOG_TRACE_POINT;

        switch (select_result) {
            case NO_CONTENT_TO_READ:
                LOG_TRACE("No content to be read on socket.");
                done_reading = true;
                result = NO_CONTENT_TO_READ;
                break;

            case GENERIC_ERROR:
                LOG_ERROR("Error while waiting for a content to read on socket.");
                done_reading = true;
                error = true;
                result = GENERIC_ERROR;
                break;

            case CONTENT_TO_READ:
                LOG_TRACE("There is content to read on socket.");

                total_read = read(socket_fd, array_pointer, sizeof(uint32_t));
                switch (total_read){
                    case 0:
                        LOG_ERROR("There is content available on socket, but it could not be read.");
                        error = true;
                        done_reading = true;
                        result = GENERIC_ERROR;
                        break;

                    case -1:
                        errno_value = errno;
                        LOG_ERROR("Error while reading socket content.");
                        LOG_ERROR("%d: %s", errno_value, strerror(errno_value));
                        /* Check if the error code returned is informing that the connection was reseted by the peer. */
                        if  ( errno_value == ERROR_CODE_CONNECTION_RESET_BY_PEER ) {
                            LOG_TRACE_POINT;
                            result = DEVICE_DISCONNECTED;
                        } else { 
                            LOG_TRACE_POINT;
                            result = GENERIC_ERROR;
                        }

                        error = true;
                        done_reading = true;
                        break;

                    default:
                        LOG_TRACE_POINT;
                        content_size += total_read;
                        if ( *array_pointer == PACKAGE_TRAILER ) {
                            LOG_TRACE("Found a package trailer.");
                            done_reading = true;
                            result = SUCCESS;
                        }
                        array_pointer = (uint32_t*)(buffer+content_size);
                        break;
                }
                LOG_TRACE("Content size: %zu byte(s).", content_size);
                break;

            default:
                LOG_ERROR("Unkown return code from \"check_socket_content\" function.");
                error = true;
                break;
        }
    }

    if ( error == true ) {
        LOG_TRACE_POINT;

        delete_byte_array(byte_array);
        LOG_TRACE_POINT;
    } else {
        copy_content_result = copy_content_to_byte_array(byte_array, buffer, content_size);
        LOG_TRACE_POINT;

        if ( copy_content_result != SUCCESS ) {
            delete_byte_array(byte_array);
            LOG_TRACE_POINT;

            result = GENERIC_ERROR;
        }
    }

    LOG_TRACE_POINT;
    return result;
}

/*
 * Writes content on socket.
 *
 * Parameters
 *  socket_fd - The socket communication file descriptor to write content.
 *  byte_array_t - The byte array content to be written on socket.
 *
 * Returns
 *  SUCCESS - If content was written successfully.
 *  GENERIC ERROR - Otherwise.
 */
int write_content_on_socket(int socket_fd, byte_array_t byte_array) {
    LOG_TRACE_POINT;

    size_t write_result;
    size_t total_written = 0;
    bool concluded = false;
    int errno_value;
    int result = SUCCESS;

    while (concluded == false ) {
        LOG_TRACE_POINT;

        write_result = write(socket_fd, byte_array.data, byte_array.size);
        switch (write_result) {
            case -1:
                errno_value = errno;
                LOG_ERROR("Error while writing content on socket.");
                LOG_ERROR("%s", strerror(errno_value));

                result = GENERIC_ERROR;
                concluded = true;
                break;

            case 0:
                LOG_TRACE_POINT;

                if ( byte_array.size != 0 ) {
                    LOG_ERROR("The content was not written on socket.");
                    result = GENERIC_ERROR;
                    concluded = true;
                }
                break;

            default:
                LOG_TRACE("%zu byte(s) written on socket.", write_result);
                break;
        }

        total_written += write_result;
        LOG_TRACE("%zu of %zu byte(s) written on socket.", total_written, byte_array.size);
        if ( total_written >= byte_array.size ) {
            LOG_TRACE_POINT;
            concluded = true;
            result = SUCCESS;
        }
    }

    LOG_TRACE_POINT;
    return result;
}
