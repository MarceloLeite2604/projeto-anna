#!/bin/bash

# This script contains generic constants used on audio functions.
#
# Version: 0.1
# Author: Marcelo Leite

# Loads generic constants script.
source "$(dirname ${BASH_SOURCE})/../generic/constants.sh";

# Path to audio confguration directory.
if [ -z "${audio_configuration_directory}" ];
then
    readonly audio_configuration_directory="${configuration_directory}audio/";
fi;

# Pipe file connecting audio capture and audio coding process.
if [ -z "${audio_pipe_file}" ];
then
    readonly audio_pipe_file=${temporary_directory}audio_pipe;
fi;

# File path to store start audio capture instant.
if [ -z "${audio_start_instant_file}" ];
then
    readonly audio_start_instant_file="${temporary_directory}start_audio_instant";
fi;

# File path to store stop audio capture instant.
if [ -z "${audio_stop_instant_file}" ];
then
    readonly audio_stop_instant_file="${temporary_directory}stop_audio_instant";
fi;
