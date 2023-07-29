#!/bin/bash
#if arguments are more than 2 or less than 2,exit
if [ "$#" -ne 1 ]; then 
    echo "Sorry wrong input ,input must be in this form:./tallyVotes.sh tallyResultsFile.txt"
    exit 1
fi
txt="inputFile.txt"
output=$1
#check if inputFile.txt exists and has read rights
if [ -e "$txt" ] && [ -r "$txt" ]; then
    declare -A parties
    declare -A voters
    while read -r line;do
        #running the inputfile and getting each element from it
        name=$(echo "$line" | awk '{print $1}')
        surname=$(echo "$line" | awk '{print $2}')
        party=$(echo "$line" | awk '{print $3}')
        if [[ " ${!voters[@]} " =~ " $name$surname " ]]; then
            continue
        else
            #add the key in voters array if not voted yet
            voters["$name$surname"]=1
            #add a vote to party
            ((parties["$party"]++))
        fi
    done < "$txt"
    #print results to tallyResultsFile
    for party in "${!parties[@]}"; do
        echo "$party ${parties[$party]}" >> "$output"
    done
else
    echo "Sorry file doesnt exist or it doesnt have read rights"
    exit 1
fi
