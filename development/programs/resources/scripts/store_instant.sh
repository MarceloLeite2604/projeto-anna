#!/bin/bash

# This script stores the current instant on a file.
#
# Parameters:
#   1. Path to file which instant must be stored.
#
# Returns:
#   SUCCESS - If instant was stored successfully.
#   GENERIC_ERROR - Otherwise.
#
# Version: 
#   0.1
#
# Author: 
#   Marcelo Leite
#


# ###
# Source scripts.
# ###

# Load generic configuration file.
source "$(dirname ${BASH_SOURCE})/generic/constants.sh";

# Load log functions.
source "$(dirname ${BASH_SOURCE})/log/functions.sh";


# ###
# Functions elaboration.
# ###

# Stores the current instant on a temporary file.
#
# Parameters:
#   1. File path to store the current instant.
#
# Returns:
#   SUCCESS - If instant was stored successfully.
#   GENERIC_ERROR - Otherwise.
#
store_current_instant(){

    local result;
    local log_file;
    local continue_log_file_result;
    local log_file_created;
    local is_log_defined_result;
    local log_file_created;
    local is_recording_result;
    local start_audio_capture_process_result;
    local start_audio_encoder_process_result;
    local file_path;

    continue_log_file_result=1;
    log_file_created=1;

    # Checks if log file is defined.
    is_log_defined;
    is_log_defined_result=${?};
    if [ ${is_log_defined_result} -eq ${success} ];
    then
        # Continues previous log file.
        continue_log_file;
        continue_log_file_result=${?};
    fi;

    # If previous log file was not continued.
    if [ ${continue_log_file_result} -ne ${success} ];
    then
        # Creates a new log file.
        create_log_file "store_current_instant";
        log_file_created=${?};

        # Set log level.
        # set_log_level ${log_message_type_trace};
    fi;

    # Checks function parameters.
    if [ ${#} -ne 1 ];
    then
        log ${log_message_type_error} "File path to store current instant is not specified.";
        return ${generic_error};
    else
        file_path="${1}";
    fi;

    # Retrieves current log file path.
    log_file="$(get_log_path)";

    # Requests to store current instant on file specified.
    $(get_binary_files_directory)store_instant "${file_path}" >> "${log_file}";
    store_current_instant_result=${?};
    if [ ${store_current_instant_result} -ne ${success} ];
    then
        log ${log_message_type_error} "Could not store current instant.";
        result=${generic_error};
    else
        log ${log_message_type_trace} "Current instant stored on file \"${file_path}\".";
        result=${success}
    fi;

    # If log file was created by this script.
    if [ ${log_file_created} -eq ${success} ];
    then

        # Finishes the log file.
        finish_log_file;
    fi;

    return ${result};
}

# Requests to store current instant.
store_current_instant "${@}";
exit ${?};
