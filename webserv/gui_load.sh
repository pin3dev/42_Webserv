#!/bin/bash

RED='\033[0;31m'     # Red
GREEN='\033[0;32m'   # Green
RESET='\033[0m'      # No color / reset
BKGRAY='\033[47;90m' # Background Gray
BKGRAYGREEN='\033[1;92m' # Background Gray and Green
BKGRAYRED='\033[1;91m' # Background Gray and Red

var=${1:-"Makefile"}

handle_error_signal()
{
    echo -e "\n${BKGRAY}${var}:${BKGRAYRED}KO!${RESET}"
    exit 1
}

trap 'handle_error_signal' SIGUSR1

var_len=${#var}
extra_chars=$((var_len + 12))

i=0  # Counter
while [ $i -le 100 ]; do
    tput clear  # Clear screen
    
    echo -en "${BKGRAY}${var}:${RESET}"
    cols=$(tput cols) # Get the number of columns in the terminal
    rounded_cols=$((cols / 2 * 2)) # Round down to the nearest even number

    bar_width=$((rounded_cols - extra_chars)) # Calculate the width of the progress bar

    # Garantee that the bar width is at least 1
    if [ $bar_width -lt 1 ]; then
        bar_width=1
    fi

    # Calculate the number of hashes (#) and spaces
    num_hashes=$(((i * bar_width) / 100))

    # Create the progress bar
    bar=$(printf "${GREEN}#%.0s${RESET}" $(seq 1 $num_hashes)) # Add hashes to the progress bar
    spaces=$((bar_width - num_hashes)) # Calculate the number of spaces
    if [ "$spaces" -gt 0 ]; then
        bar="${bar}$(printf ' %.0s' $(seq 1 $spaces))" # Add spaces to the progress bar if needed
    fi

    # Print progress bar
    printf "[%-*s] %3d%%" "$bar_width" "$bar" "$i"

    sleep 0.1  # Sleep for 0.1 seconds
    i=$((i+10))  # Update counter
done
echo -e "\n${BKGRAY}${var}:${BKGRAYGREEN}OK!${RESET}"