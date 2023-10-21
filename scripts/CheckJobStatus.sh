#!/bin/sh

while true; do
    # Run the 'condor_q' command, get the last 3 lines of its output, and then get the first line
    output=$(condor_q restricted-list tpani | tail -n 3 | head -n 1)

    # Extract the number just before the word 'running'
    running_jobs=$(echo "$output" | grep -oP '\d+(?=\srunning)')

    # Print the result
    echo "Number of running jobs: $running_jobs"

    # Break the loop if the number of running jobs is 0
    if [ "$running_jobs" -eq 0 ]; then
        break
    fi
  
    # Sleep for 10 minutes (600 seconds)
    sleep 600
done

