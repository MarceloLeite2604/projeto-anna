#!/bin/bash

# This script contains all functions and variables required to log and trace functions.
#
# Version: 0.1
# Author: Marcelo Leite

# Load configuration file.
source $(dirname $BASH_SOURCE)/configuration.sh

# Load constants used by log functions.
source $(dirname $BASH_SOURCE)/log_constants.sh

# Load general functions.
source $(dirname $BASH_SOURCE)/general_functions.sh

# Log directory.
log_directory="log/";

# Log file location.
log_file_location="";

# Indicates if the log file creation was successful.
log_file_creation_result=${not_executed};

# Indicates the log level.
log_level=${type_warning};

# Writes a message on "stderr".
#
# Parameters
#  1. Message to be written.
#
# Returns
#  Code returned from "echo" writing on "stderr".
write_stderr(){

    for parameter in "$@"
    do
        local stderr_message=${stderr_message}${parameter}" ";
    done;

    local result=$success;
    if [ -n "${stderr_message}" ]; then
        $(>&2 echo ${stderr_message})
        local result=$?;
    fi;

    return $result;
}

# Writes a message on a file.
#
# Parameters
#  1. File path to write the message.
#  2. Message to be written.
#
# Returns
#    0. If message was successfully written.
#   -1. Otherwise.
write_on_file(){

    if [ $# -ne 2 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Not enough parameters to execute \"${FUNCNAME[O]}\".";
        return ${general_failure};
    fi;

    local file_path="$1";
    local message="$2";

    # If the file does not exists.
    if [ ! -f ${file_path} ];
    then
        write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Could not find file \"${file_path}\".";
        return ${general_failure};
    fi;

    # Check if user was write permission on file.
    if [ ! -w ${file_path} ];
    then
        write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: User \"$(whoami)\" does not have write permission on  \"${file_path}\".";
        return ${general_failure};
    fi;

    # Write message on file.
    echo "${message}" >> ${file_path};
    local echo_result=$?;
    if [ $echo_result -ne 0 ];
    then
        write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Error writing message on \"${file_path}\" (${echo_result}).";
        return ${general_failure};
    fi; 
}


# Creates a log filename based on preffix informed and current time.
#
# Parameters
#	1. Log file preffix.
#
# Returns
#   0. If log filename was created sucessfully.
#  -1. Otherwise
#
#	It also returns the log file name created trough "echo".
create_log_file_name() {

    if [ $# -ne 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_failure};
    fi;

    local log_file_preffix="$1";

    local log_file_name="${log_file_preffix}_$(get_current_time_formatted).${log_file_suffix}";

    echo "$log_file_name";

    return ${success};
}

# Creates the log file based on a preffix informed.
#
# Parameters
#   1. Preffix to identify log file.	
#
# Returns
#	0. If log file was created.
#  -1. Otherwise.
create_log_file() {

    if [ $# -ne 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        log_file_location="";
        log_file_creation_result=${general_failure};
        return ${general_failure};
    fi;
    local readonly log_file_preffix="$1";
    local readonly log_file_name=$(create_log_file_name $log_file_preffix);

    local readonly temporary_log_file_location=${log_directory}${log_file_name};

    # If log file does not exist.
    if [ ! -f ${temporary_log_file_location} ];
    then

        # If log directory does not exist.
        if [ ! -d "${log_directory}" ];
        then
            write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Invalid log directory \"${log_directory}\".";
            log_file_location="";
            log_file_creation_result=${general_failure};
            return ${general_failure};
        fi;

        # If user does not have write permission on log directory.
        if [ ! -w "${log_directory}" ];
        then
            write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: User \"$(whoami)\" does not have write permission on directory. \"${log_directory}\".";
            log_file_location="";
            log_file_creation_result=${general_failure};
            return ${general_failure};
        fi;

        # Creates the log file.
        touch ${temporary_log_file_location};

        # If, after creation, the file is still missing.
        if [ ! -f "${temporary_log_file_location}" ];
        then
            write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Could not create file \"${temporary_log_file_location}\".";
            write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Log messages will be redirected to \"stderr\".";
            log_file_location="";
            log_file_creation_result=${general_failure};
            return ${general_failure};
        fi;
    fi;

    # If user does not have write permission on log file.
    if [ ! -w "${temporary_log_file_location}" ];
    then
        write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: User \"$(whoami)\" does not have write access on \"${temporary_log_file_location}.";
        write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${error_preffix}: Log messages will be redirected to \"stderr\".";
        log_file_location="";
        log_file_creation_result=${general_failure};
    fi;

    log_file_location=${temporary_log_file_location};
    log_file_creation_result=${success};

    write_on_file ${log_file_location} "[$(get_current_time)] Log started.";
    return ${success};

}

# Finishes log file.
#
# Parameters
#  None.
#
# Returns
#   0. If log was finished successfully.
#  -1. Otherwise.
finish_log_file(){

    if [ $# -ne 0 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_failure};
    fi;
    if [ -z "${log_file_location}" -o ${log_file_creation_result} -ne ${success} ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: No log file to finish.";
        return ${general_failure};
    fi;
    local readonly finish_message="[$(get_current_time)] Log finished.";

    write_on_file ${log_file_location} "${finish_message}";

    if [ $? -ne ${success} ];
    then
        return ${general_failure};
    fi;

    unset log_file_location;
    log_file_creation_result=${not_executed};

    return ${success};
}

# Writes a message on log file.
#
# Parameters
#   1. Message to be written.
#
# Returns
#    0. If message was correctly written.
#   -1. Otherwise.
write_log_message() {

    if [ $# -ne 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_failure};
    fi;

    local message_content="[$(get_current_time)] $1";
    local write_on_stderr="false";

    # If log file location is not specified.
    if [ -z ${log_file_location+x} ];
    then
        # If log file creation was not executed.
        if [ $log_file_creation_result -eq $not_executed ];
        then
            # Creates log file.
            create_log_file "log";

            # If there was an error on log creation.
            if [ $? -ne $success ];
            then
                local $write_on_stderr="true";
            else
                write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${warning_preffix}: No log file was prevously specificated.";
                write_stderr "[${FUNCNAME[0]}, ${LINENO[0]}] ${warning_preffix}: Created new log file \"${log_file_location}\".";
            fi;

        else
            local $write_on_stderr="true";
        fi;
    fi;

    if [ "$write_on_stderr" = "true" ];
    then
        write_stderr ${message_content};
    else
        # Print message on log file.    
        echo "${message_content}" >> ${log_file_location};
    fi;
}

# Log a message.
#
# Parameters
#   1. Message type (trace, info, warning, error).
#   2. Message content. (optional for trace messages, mandatory for other message types).
#
# Returns
#   0. If message was successfully logged.
#  -1. Otherwise.
log() {

    # Check function parameters.
    if [ $# -lt 1 -o $# -gt 2 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_error};
    fi;

    local readonly message_type=$1;
    local readonly message_content="$2";

    case $message_type in
        $type_trace)
            local readonly preffix=$trace_preffix;
            ;;
        $type_warning)
            local readonly preffix=$warning_preffix;
            ;;
        $type_error)
            local readonly preffix=$error_preffix;
            ;;
        *)
            write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid log message type (${message_type}).";
            return ${general_error};
            ;;
    esac;

    local message_tag="${FUNCNAME[1]}";
    local message_index=${BASH_LINENO[0]};

    if [ "${message_tag}" == "${trace_function_name}" ];
    then
        message_tag=${FUNCNAME[2]};
        message_index=${BASH_LINENO[1]};
    fi;

    if [ $message_type -ne $type_trace -a -z "$message_content" ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Only trace log messages are allowed to be written without content.";
        return ${general_failure};
    fi;

    # If message level is lower than current log level.
    if [ ${message_type} -lt ${log_level} ];
    then
        # Then the message should not be logged.
        return ${success};
    fi;

    local log_message="${preffix}: ${message_tag} (${message_index})";

    if [ -n "${message_content}" ];
    then
        local log_message=${log_message}": ${message_content}";
    fi;

    write_log_message "${log_message}";

    local result=$?
    return ${result};
}

# This function register a trace point on log file.
#
# Parameters
#   1. Message (optional). 
#
# Returns
#   0. If trace was logged correctly.
#  -1. Otherwise.
trace(){

    if [ $# -gt 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_failure};
    fi;

    if [ $# -eq 1 ];
    then
        local readonly trace_message="$1";
    fi;

    log ${type_trace} "${trace_message}";

    return $?;
}

# Set the log level.
#
# Parameters
#   1. The log level to be defined.
#
# Returns
#   0. If log level was defined correctly.
#  -1. Otherwise.
#
# Observation
#   To define correctly the log level, use the constants defines on
# "log_constants.sh".
set_log_level(){

    local readonly numeric_regex="^[0-9]+$";

    if [ $# -ne 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_failure};
    fi;

    local readonly temporary_log_level=$1;

    if ! [[ ${temporary_log_level} =~ ${numeric_regex} ]];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid log level value (${temporary_log_level}).";
        return ${general_failure};
    fi;

    if [ ${temporary_log_level} -ne ${type_trace} -a ${temporary_log_level} -ne ${type_warning} -a ${temporary_log_level} -ne ${type_error} ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid log level value (${temporary_log_level}).";
        return ${general_failure};
    fi;

    log_level=${temporary_log_level};

    return ${success};
}

# Set the directory to write log files.
#
# Parameters
#   1. Relative or complete path to directory to store log files.
#
# Returns
#   0. If log directory was updated correctly.
#  -1. Otherwise.
#
# Observations
#   If the directory does not exists, this function willi not create it. 
# It will return and error and log directory will not be updated.
set_log_directory(){

    if [ $# -ne 1 ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Invalid parameters to execute \"${FUNCNAME[0]}\".";
        return ${general_error};
    fi;

    local readonly temporary_log_directory="$1";

    if [ ! -d "${temporary_log_directory}" ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: Unknown or invalid directory \"${temporary_log_directory}\".";
        return ${general_error};
    fi;

    if [ ! -w "${temporary_log_directory}" ];
    then
        write_stderr "[${FUNCNAME[1]}, ${BASH_LINENO[0]}] ${error_preffix}: User \"$(whoami)\" does not have write permission on directory \"${temporary_log_directory}\".";
        return ${general_error};
    fi;

    log_directory=${temporary_log_directory};
    return ${success};
}
