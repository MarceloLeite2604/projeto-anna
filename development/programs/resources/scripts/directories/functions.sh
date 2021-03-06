#!/bin/bash

# This script contains the functions required to obtain the input, output and 
# binary directories for script executions.
#
# Parameter:
#   None.
#
# Returns:
#   None.
#
# Version:
#   0.1
#
# Author: 
#   Marcelo Leite
#

# ###
# Include guard.
# ###
if [ -z "${DIRECTORIES_FUNCTIONS_SH}" ];
then
    DIRECTORIES_FUNCTIONS_SH=1;
else
    return;
fi;


# ###
# Script sources.
# ###

# Load generic constants script.
source "$(dirname ${BASH_SOURCE})/../generic/constants.sh";

# Load directory constants script.
source "$(dirname ${BASH_SOURCE})/constants.sh";


# ###
# Function elaborations.
# ###

# Returns the binaries directory path.
#
# Parameters:
#   None.
#
# Returns:
#   SUCCESS - If binaries directory was acquired and returned correctly.
#   GENERIC_ERROR - Otherwise.
#   It also returns the binaries directory path through "echo".
#
get_binary_files_directory(){
    local result;

    # Checks if binaries directory file exists.
    if [ ! -f "${_directories_binaries_directory_file}" ];
    then
        $(>&2 echo "Could not find binaries directory file \"${_directories_binaries_directory_file}\".");
        return ${generic_error};
    fi;

    # Reads binaries directory file content.
    result="$(cat "${_directories_binaries_directory_file}")";
    if [ -z "${result}" ];
    then
        $(>&2 echo "Binaries directory file \"${_directories_binaries_directory_file}\" is empty.");
        return ${generic_error};
    fi;

    result="$(dirname ${BASH_SOURCE})/${result}";

    echo "${result}";
    return ${success};
}

# Returns the input files directory path.
#
# Parameters:
#   None.
#
# Returns:
#   SUCCESS - If input directory was acquired and returned correctly.
#   GENERIC_ERROR - Otherwise.
#   It also returns the input files directory path through "echo".
#
get_input_files_directory(){

    local result;

    # Checks if input directory file exists.
    if [ ! -f "${_directories_input_directory_file}" ];
    then
        $(>&2 echo "Could not find input directory file \"${_directories_input_directory_file}\".");
        return ${generic_error};
    fi;

    # Reads input directory file content.
    result=$(cat "${_directories_input_directory_file}");
    if [ -z "${result}" ];
    then
        $(>&2 echo "Input directory file \"${_directories_input_directory_file}\" is empty.");
        return ${generic_error};
    fi;

    result="$(dirname ${BASH_SOURCE})/${result}";

    echo "${result}";
    return ${success};
}

# Returns the output files directory path.
#
# Parameters:
#   None.
#
# Returns:
#   SUCCESS - If output directory was acquired and returned correctly.
#   GENERIC_ERROR - Otherwise.
#   It also returns the output files directory path through "echo".
#
get_output_files_directory(){

    local result;

    # Checks if output directory file exists.
    if [ ! -f "${_directories_output_directory_file}" ];
    then
        $(>&2 echo "Could not find output directory file \"${_directories_output_directory_file}\".");
        return ${generic_error};
    fi;

    # Reads output directory file content.
    result=$(cat "${_directories_output_directory_file}");
    if [ -z "${result}" ];
    then
        $(>&2 echo "Output directory file \"${_directories_output_directory_file}\" is empty.");
        return ${generic_error};
    fi;

    result="$(dirname ${BASH_SOURCE})/${result}";

    echo "${result}";
    return ${success};
}
