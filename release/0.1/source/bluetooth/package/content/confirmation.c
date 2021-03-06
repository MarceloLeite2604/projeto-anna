/*
 * This source file contains the elaboration of all components required to create and manipulate "confirmation" package contents.
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

#include <stdlib.h>

#include "bluetooth/package/content/confirmation.h"
#include "log.h"
#include "return_codes.h"


/*
 * Function elaborations.
 */

/*
 * Converts a byte array to a "confirmation" package content.
 *
 * Parameters
 *  confirmation_content - The variable where the "confirmation" package content will be stored.
 *  byte_array - The byte array with information of the "confirmation" package content.
 *
 * Returns
 *  SUCCESS - If the byte array was converted successfully.
 *  GENERIC ERROR - Otherwise.
 */
int convert_byte_array_to_confirmation_content(confirmation_content_t* confirmation_content, byte_array_t byte_array) {
    LOG_TRACE_POINT;

    size_t content_size;

    content_size = sizeof(uint32_t);
    if ( byte_array.size != content_size ) {
        LOG_ERROR("The byte array size does not match a confirmation content.");
        return GENERIC_ERROR;
    }

    memcpy(&confirmation_content->package_id, byte_array.data, sizeof(uint32_t));

    LOG_TRACE_POINT;
    return SUCCESS;
}

/*
 * Creates a "confirmation" package content.
 *
 * Parameters
 *  package_id - The package id to confirm its delivery.
 *
 * Returns
 *  A "confirmation" package content with the id of the delivered package.
 */
confirmation_content_t* create_confirmation_content(uint32_t package_id){
    LOG_TRACE("Package id: 0x%x.", package_id);

    confirmation_content_t* confirmation_content;

    confirmation_content = (confirmation_content_t*)malloc(sizeof(confirmation_content_t));
    confirmation_content->package_id = package_id;

    LOG_TRACE_POINT;
    return confirmation_content;
}

/*
 * Creates a byte array containing the "confirmation" package content.
 *
 * Parameters
 *  confirmation_content - The "confirmation" package content with the informations to build the byte array.
 *
 * Returns
 *  A byte array structure with the confirmation package content informations.
 */
byte_array_t create_confirmation_content_byte_array(confirmation_content_t confirmation_content) {
    LOG_TRACE_POINT;

    byte_array_t byte_array;

    byte_array.size = sizeof(uint32_t);
    byte_array.data = (uint8_t*)malloc(byte_array.size*sizeof(uint8_t));
    memcpy(byte_array.data, &confirmation_content.package_id, sizeof(uint32_t));

    LOG_TRACE_POINT;
    return byte_array;
}

/*
 * Deletes a "confirmation" package content.
 *
 * Parameters
 *  confirmation_content - The "confirmation" package content to be deleted.
 *
 * Returns
 *  SUCCESS - If the content was deleted successfully.
 *  GENERIC ERROR - Otherwise.
 */
int delete_confirmation_content(confirmation_content_t* confirmation_content) {
    free(confirmation_content);

    LOG_TRACE_POINT;
    return SUCCESS;
}
