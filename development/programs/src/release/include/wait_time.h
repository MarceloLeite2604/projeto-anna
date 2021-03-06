/*
 * This header file contains all components required to control waiting times and retry attempts.
 *
 * Version: 
 *  0.1
 *
 * Author: 
 *  Marcelo Leite
 */

#ifndef WAIT_TIME_H
#define WAIT_TIME_H


/*
 * Macros.
 */

/* Code used to indicate that retry controller reached its maximum attempts. */
#define MAXIMUM_RETRY_ATTEMPTS_REACHED 53


/*
 * Structures.
 */

/* Stores informations about retry attempts. */
typedef struct {
    int maximum;
    int attempts;
} retry_informations_t;


/*
 * Function headers.
 */

/* Creates a retries infos structure. */
retry_informations_t create_retry_informations(int);

/* Waits an amount of time base on retry attempts. */
int wait_time(retry_informations_t*);

#endif
