/*
 * This header file contains all error messages used by the program.
 *
 * Version:
 *  0.1
 *
 * Author: 
 *  Marcelo Leite
 */

#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H


/*
 * Macros.
 */

/* Error messages used by the program. */
#define ERROR_MESSAGE_000 "No error."
#define ERROR_MESSAGE_001 "Could not start log."
#define ERROR_MESSAGE_002 "Could not start bluetooth service."
#define ERROR_MESSAGE_003 "Could not send confirmation message."
#define ERROR_MESSAGE_004 "Record program already running."
#define ERROR_MESSAGE_005 "Failed to start record program."
#define ERROR_MESSAGE_006 "Failed to transmit result."
#define ERROR_MESSAGE_007 "Maximum attempts reached while trying to write data on socket."
#define ERROR_MESSAGE_008 "No confirmation received."
#define ERROR_MESSAGE_009 "Record program not running."
#define ERROR_MESSAGE_010 "Could not stop record program gracefully."
#define ERROR_MESSAGE_011 "Device is recording."
#define ERROR_MESSAGE_012 "No audio record file found."
#define ERROR_MESSAGE_013 "Could not transmit audio record."
#define ERROR_MESSAGE_014 "Connection lost."
#define ERROR_MESSAGE_015 "Maximum attempts reached while trying to read content from socket."
#define ERROR_MESSAGE_016 "Failed to finish communication."
#define ERROR_MESSAGE_017 "Failed to unregister bluetooth service."
#define ERROR_MESSAGE_018 "Failed to transmit error message."
#define ERROR_MESSAGE_019 "Could not send file header."
#define ERROR_MESSAGE_020 "Could not send file content."
#define ERROR_MESSAGE_021 "Could not send file trailer."
#define ERROR_MESSAGE_022 "Could not close connection gracefully."


/*
 * Constants.
 */

/* The array of error messages. */
extern const char* error_messages[];

#endif
