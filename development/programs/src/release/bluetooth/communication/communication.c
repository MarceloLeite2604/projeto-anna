/*
 * This source file contains all component elaborations required to control the bluetooth communication.
 *
 * Version: 0.1
 * Author: Marcelo Leite
 */

/*
 * Includes.
 */
#include <libgen.h>
#include <errno.h>
#include "../../general/wait_time/wait_time.h"
#include "../../general/file/file.h"
#include "../package/codes/codes.h"
#include "communication.h"


/*
 * Definitions.
 */

/* Maximum attempts to write a package content on a connection socket. */
#define MAXIMUM_WRITE_ATTEMPTS 30

/* Maximum attempts to read a package content on a connection socket. */
#define MAXIMUM_READ_ATTEMPTS 30

/* Size of the buffer to store file data chunks. */
#define DATA_CHUNK_BUFFER_SIZE 1024

/*
 * Function headers.
 */

/* Receives a confirmation package. */
int receive_confirmation(int, package_t);

/* Sends a confirmation package. */
int send_confirmation(int, package_t);

/* Sends a file content. */
int send_file_content(int, char*, size_t);

/* Sends a file chunk. */
int send_file_chunk(int, size_t, uint8_t*);

/* Sends a file header. */
int send_file_header(int, size_t, const char*);

/* Sends a file trailer. */
int send_file_trailer(int);

/* Sends a package through a connection. */
int send_package(int, package_t);


/*
 * Function elaborations.
 */

/*
 * Checks if a device is connected.
 *
 * Parameters
 *  socket_fd - The device's connection socket file descriptor.
 *
 * Returns
 *  0. If the device responded the connection check.
 *  1. If the device did not respond the connection check or there was an error while chekcing connection.
 */
int check_connection(int socket_fd) {
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;

    package_t check_connection_package;
    byte_array_t check_connection_package_byte_array;

    check_connection_package = create_check_connection_package();
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, check_connection_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending a check connection package.");
        result = 1;
    }

    delete_package(check_connection_package);

    LOG_TRACE_POINT;
    return result;
}

/*
 * Receives a confirmation package.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to receive the confirmation.
 *  package   - The package awaiting to be confirmed.
 *
 * Returns
 *  0. If the confirmation package was received successfully.
 *  1. Otherwise.
 */
int receive_confirmation(int socket_fd, package_t package) {
    LOG_TRACE_POINT;

    bool read_concluded = false;
    byte_array_t byte_array_readed;
    retry_informations_t retry_informations;
    package_t package_received;
    int wait_result;
    int convertion_result;
    int result;

    retry_informations = create_retry_informations(MAXIMUM_READ_ATTEMPTS);
    LOG_TRACE_POINT;

    while ( read_concluded == false ) {
        LOG_TRACE_POINT;

        delete_byte_array(byte_array_readed);
        LOG_TRACE_POINT;

        byte_array_readed = read_socket_content(socket_fd);
        LOG_TRACE_POINT;

        if ( byte_array_readed.size > 0 ) {
            LOG_TRACE_POINT;

            delete_package(package_received);
            LOG_TRACE_POINT;

            convertion_result = convert_byte_array_to_package(&package_received, byte_array_readed);
            LOG_TRACE_POINT;

            if ( convertion_result == 1 ) {
                LOG_TRACE_POINT;
                LOG_ERROR("Error while converting byte array to package.");
                result = 1;
                read_concluded = true;
            }
            else {
                LOG_TRACE_POINT;

                if ( package_received.type_code == CONFIRMATION_CODE ) {
                    LOG_TRACE_POINT;
                    if ( package_received.content.confirmation_content->package_id == package.id ) {
                        LOG_TRACE_POINT;
                        read_concluded = true;
                        result = 0;
                    }
                }
            }
        }

        if ( read_concluded == false ) {
            LOG_TRACE_POINT;

            wait_result = wait_time(&retry_informations);
            LOG_TRACE_POINT;

            switch (wait_result) {
                case -1:
                    LOG_ERROR("Error while waiting to read the package confirmation from connection.");
                    read_concluded = true;
                    result = 1;
                    break;
                case 1:
                    LOG_ERROR("Maximum read attempts reached.");
                    read_concluded = true;
                    result = 1;
                    break;
                default:
                    LOG_TRACE_POINT;
                    break;
            }
        }
    }

    delete_byte_array(byte_array_readed);
    LOG_TRACE_POINT;

    delete_package(package_received);
    LOG_TRACE_POINT;

    return result;
}

/*
 * Receives a package from a connection.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptot to send the package.
 *  package - Returns the package received.
 *
 * Returns
 *  0. If package was received.
 *  1. If there was an error receiving the package.
 *  2. If no package was received.
 */
int receive_package(int socket_fd, package_t* package) {
    LOG_TRACE_POINT;

    int result = 0;
    byte_array_t received_byte_array;
    retry_informations_t retry_informations;
    bool receive_concluded;
    int wait_result;
    int convertion_result;

    retry_informations = create_retry_informations(MAXIMUM_READ_ATTEMPTS);
    LOG_TRACE_POINT;

    while ( receive_concluded == false ) {
        LOG_TRACE_POINT;

        delete_byte_array(received_byte_array);
        LOG_TRACE_POINT;

        received_byte_array = read_socket_content(socket_fd);
        LOG_TRACE_POINT;

        if ( received_byte_array.size != 0 ) {
            LOG_TRACE_POINT;
            receive_concluded = true;
        }
        else {
            LOG_TRACE_POINT;

            wait_result = wait_time(&retry_informations);
            LOG_TRACE_POINT;

            switch (wait_result) {
                case -1:
                    LOG_ERROR("Error while waiting to receive a package.");
                    receive_concluded = true;
                    result = 1;
                    break;
                case 1:
                    LOG_TRACE("Maximum receive attempts reached.");
                    receive_concluded = true;
                    result = 2;
                    break;
                default:
                    LOG_TRACE_POINT;
                    break;
            }
        }
    }

    if ( received_byte_array.size != 0 ) {
        LOG_TRACE_POINT;

        convertion_result = convert_byte_array_to_package(package, received_byte_array);
        LOG_TRACE_POINT;

        if ( convertion_result == 1 ) {
            LOG_ERROR("Error while converting received byte array to package.");
            result = 1;
        }
    }

    delete_byte_array(received_byte_array);
    LOG_TRACE_POINT;

    return result;
}

/*
 * Sends a confirmation package.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptot to send the package.
 *  package - The package to be confirmed.
 *
 * Returns
 *  0. If the confirmation package was send successfully.
 *  1. Otherwise
 */
int send_confirmation(int socket_fd, package_t package_to_confirm) {
    LOG_TRACE_POINT;

    int result;
    package_t confirmation_package;
    byte_array_t confirmation_package_byte_array;
    bool send_concluded = false;
    int write_result;
    int wait_result;
    int convertion_result;
    retry_informations_t retry_informations;

    confirmation_package = create_confirmation_package(package_to_confirm.id);
    LOG_TRACE_POINT;

    convertion_result = convert_package_to_byte_array(&confirmation_package_byte_array, confirmation_package);
    LOG_TRACE_POINT;

    if ( convertion_result == 1 ) {
        LOG_ERROR("Error converting confirmation package to byte array.");
        result = 1;
    }
    else {
        LOG_TRACE_POINT;

        retry_informations = create_retry_informations(MAXIMUM_WRITE_ATTEMPTS);
        LOG_TRACE_POINT;

        while ( send_concluded == false ) {
            LOG_TRACE_POINT;

            write_result = write_content_on_socket(socket_fd, confirmation_package_byte_array); 
            LOG_TRACE_POINT;

            if ( write_result == 1 ) {
                LOG_ERROR("Error while writing confirmaton package on socket.");
                wait_result = wait_time(&retry_informations);
                LOG_TRACE_POINT;
                switch (wait_result) {
                    case -1:
                        LOG_ERROR("Error while waiting to read the package confirmation from connection.");
                        send_concluded = true;
                        result = 1;
                        break;
                    case 1:
                        LOG_ERROR("Maximum read attempts reached.");
                        send_concluded = true;
                        result = 1;
                        break;
                    default:
                        LOG_TRACE_POINT;
                        break;
                }
            }
            else {
                LOG_TRACE_POINT;

                send_concluded = true;
                result = 0;
            }
        }
    }

    delete_package(confirmation_package);
    LOG_TRACE_POINT;

    delete_byte_array(confirmation_package_byte_array);
    LOG_TRACE_POINT;

    return result;
}

/*
 * Sends a package through a connection.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to send the package.
 *  package   - The package to be sent.
 *
 *  Returns
 *   0. If the package was sent successfuly.
 *   1. Otherwise.
 */
int send_package(int socket_fd, package_t package) {
    LOG_TRACE_POINT;

    int result = 1;
    int write_result;
    int wait_result;
    int receive_confirmation_result;
    int convertion_result;
    bool write_concluded = false;
    retry_informations_t retry_informations;
    byte_array_t package_byte_array;

    retry_informations = create_retry_informations(MAXIMUM_WRITE_ATTEMPTS);
    LOG_TRACE_POINT;

    convertion_result = convert_package_to_byte_array(&package_byte_array, package);
    LOG_TRACE_POINT;

    if ( convertion_result == 1 ) {
        LOG_ERROR("Error while converting package to byte array.");
        return 1;
    }

    while (write_concluded == false ) {
        LOG_TRACE_POINT;

        write_result = write_content_on_socket(socket_fd, package_byte_array);

        if ( write_result == 0 ) {
            LOG_TRACE_POINT;

            receive_confirmation_result = receive_confirmation(socket_fd, package);
            LOG_TRACE_POINT;

            if ( receive_confirmation_result == 0 ) {
                LOG_TRACE_POINT;
                write_concluded = true;
                result = 0;
            }
            else {
                LOG_ERROR("Did not receive confirmation o package id 0x%x.", package.id);
                write_concluded = true;
                result = 1;
            }
        } else {
            LOG_TRACE_POINT;

            wait_result = wait_time(&retry_informations);
            LOG_TRACE_POINT;

            switch (wait_result) {
                case -1:
                    LOG_ERROR("Error while waiting to retry writing package.");
                    write_concluded = true;
                    result = 1;
                    break;
                case 1:
                    LOG_ERROR("Maximum write attempts reached.");
                    write_concluded = true;
                    result = 1;
                    break;
                default:
                    LOG_TRACE_POINT;
                    break;

            }
        }
    }

    delete_byte_array(package_byte_array);
    LOG_TRACE_POINT;
    return result;
}

/*
 * Sends a file through a connection.
 *
 * Paramters
 *  socket_fd - The connection socket file descriptor to send the file.
 *  file_path - Path to the file to be sent.
 *
 * Returns
 *  0. If the file was sent successfully.
 *  1. Otherwise.
 */
int send_file(int socket_fd, char* file_path) {
    LOG_TRACE_POINT;

    char* file_name;
    size_t file_size;
    int send_result;

    if ( file_exists(file_path) == false ) {
        LOG_ERROR("File \"%s\" does not exist.", file_path);
        return 1;
    }

    if ( file_is_readable(file_path) == false ) {
        LOG_ERROR("File \"%s\" is not readable.", file_path);
        return 1;
    }

    file_size = get_file_size(file_path);
    LOG_TRACE_POINT;

    if ( file_size == -1 ) {
        LOG_ERROR("Could not determine the \"%s\" file size.", file_path);
        return 1;
    }

    file_name = basename(file_path);
    LOG_TRACE("File name: \"%s\".", file_name);

    send_result = send_file_header(socket_fd, file_size, file_name);
    LOG_TRACE_POINT;

    if ( send_result == 1 ) {
        LOG_ERROR("Error while sending file header.");
        return 1;
    }

    send_result = send_file_content(socket_fd, file_name, file_size);
    LOG_TRACE_POINT;

    if ( send_result == 1 ) {
        LOG_ERROR("Error while sending file content.");
        return 1;
    }

    send_result = send_file_trailer(socket_fd);
    LOG_TRACE_POINT;

    if ( send_result == 1 ) {
        LOG_ERROR("Error while sending file trailer.");
        return 1;
    }

    LOG_TRACE_POINT;
    return 0;
}

/*
 * Sends a file content.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to send the file content.
 *  file_path - The file path to send its content. 
 *
 * Returns
 *  0. If the content was sent successfully.
 *  1. Otherwise.
 */
int send_file_content(int socket_fd, char* file_path, size_t file_size) {
    LOG_TRACE_POINT;

    bool send_content_concluded;
    int errno_value;
    uint8_t data_chunk_buffer[DATA_CHUNK_BUFFER_SIZE];
    size_t bytes_read;
    size_t total_bytes_read; 
    FILE* file;
    int fclose_result;
    int send_result;
    int result;

    file = fopen(file_path, "r");

    if ( file == NULL ) {
        errno_value = errno;
        LOG_ERROR("Could not open file \"%s\".", file_path);
        LOG_ERROR("%s", strerror(errno_value));
        return 1;
    }


    while (send_content_concluded == false ) {
        LOG_TRACE_POINT;

        bytes_read = fread(data_chunk_buffer, sizeof(uint8_t), DATA_CHUNK_BUFFER_SIZE, file);

        if ( ferror(file) != 0 ) {
            LOG_ERROR("Error while reading \"%s\" file data chunk.", file_path);
            send_content_concluded = true;
            result = 1;
        }

        total_bytes_read += bytes_read;
        LOG_TRACE("Bytes read: %zu, total bytes read: %zu.", bytes_read, total_bytes_read);

        if ( feof(file) != 0 && total_bytes_read != file_size ) {
            LOG_TRACE_POINT;
            send_content_concluded = true;

            if ( total_bytes_read != file_size ) {
                LOG_ERROR("File reading concluded, but the total bytes sent is different from file size");
                LOG_ERROR("File size: %zu, total read: %zu.", file_size, total_bytes_read);
                result = 1;
            } else {
                LOG_TRACE_POINT;
                result = 0;
            }
        }

        send_result = send_file_chunk(socket_fd, bytes_read, data_chunk_buffer);
        LOG_TRACE_POINT;

        if ( send_result == 1 ) {
            LOG_ERROR("Error while sending file data chunk.");
            send_content_concluded = true;
            result = 1;
        }
    }

    fclose_result = fclose(file);

    if ( fclose_result != 0 ) {
        errno_value = errno;
        LOG_ERROR("Error while closing file \"%s\".", file_path);
        LOG_ERROR("%s", strerror(errno_value));
        return 1;
    }

    LOG_TRACE_POINT;
    return result;
}

/*
 * Sends a file chunk.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to send the file chunk.
 *  chunk_size - Size of the chunk to be sent.
 *  chunk_data - The chunk of data to be sent.
 *
 * Returns
 *  0. If the file chunk was sent successfully.
 *  1. Otherwise.
 */
int send_file_chunk(int socket_fd, size_t chunk_size, uint8_t* chunk_data){
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;
    package_t send_file_chunk_package;

    send_file_chunk_package = create_send_file_chunk_package(chunk_size, chunk_data);
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, send_file_chunk_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending the file chunk.");
        result = 1;
    }

    delete_package(send_file_chunk_package);
    LOG_TRACE_POINT;
    return result;
}

/*
 * Sends a file header.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to send the file header.
 *  file_size - Size of the file to be sent.
 *  file_name - The name of the file to be sent.
 *
 * Returns
 *  0. If the file header was sent successfully.
 *  1. Otherwise.
 */
int send_file_header(int socket_fd, size_t file_size, const char* file_name){
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;
    package_t send_file_header_package;

    send_file_header_package = create_send_file_header_package(file_size, file_name);
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, send_file_header_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending the file header.");
        result = 1;
    }

    delete_package(send_file_header_package);
    LOG_TRACE_POINT;
    return result;
}

/*
 * Sends a file trailer.
 *
 * Parameters
 *  socket_fd - The connection socket file descriptor to send the file trailer.
 *
 * Returns
 *  0. If the file trailer was sent successfully.
 *  1. Otherwise.
 */
int send_file_trailer(int socket_fd){
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;
    package_t send_file_trailer_package;

    send_file_trailer_package = create_send_file_trailer_package();
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, send_file_trailer_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending the file trailer.");
        result = 1;
    }

    delete_package(send_file_trailer_package);
    LOG_TRACE_POINT;
    return result;
}

/*
 * Transmits a command result.
 *
 * Parameters
 *  
 *  socket_fd - The connection socket file descriptor to send the command result.
 *  command_result - The command result to be sent.
 *
 * Returns
 *  0. If the command result was send sucessfully.
 *  1. Otherwise.
 */
int transmit_command_result(int socket_fd, int command_result) {
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;
    package_t command_result_package;

    command_result_package = create_command_result_package(command_result);
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, command_result_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending the command result.");
        result = 1;
    }

    delete_package(command_result_package);
    LOG_TRACE_POINT;

    return result;
}

/*
 * Transmits an error.
 *
 * Parameters
 *  
 *  socket_fd - The connection socket file descriptor to send the command result.
 *  error_code - The error code to be sent.
 *  error_message - The error message to be send.
 *
 * Returns
 *  0. If the error message was transmitted successfully.
 *  1. Otherwise.
 */
int transmit_error(int socket_fd, int error_code, const char* error_message) {
    LOG_TRACE_POINT;

    int result = 0;
    int send_package_result;
    package_t error_package;

    error_package = create_error_package(error_code, error_message);
    LOG_TRACE_POINT;

    send_package_result = send_package(socket_fd, error_package);
    LOG_TRACE_POINT;

    if ( send_package_result == 1 ) {
        LOG_ERROR("Error while sending the error message.");
        result = 1;
    }

    delete_package(error_package);

    LOG_TRACE_POINT;
    return result;
}