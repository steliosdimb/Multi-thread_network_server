#!/bin/bash
#if arguments more than 2 or less than 2 error message
if [ "$#" -ne 2 ]; then
    echo "Sorry wrong input, input must be in this form: ./create_input.sh politicalParties.txt numLines"
    exit 1
fi

txt=$1
num_lines=$2
output_file="inputFile.txt"

declare -a parties
i=0
#read every line from file and store it to the array
while IFS= read -r line; do
    parties[$i]="$line"
    i=$((i+1))
done < "$txt"

for ((i=0; i<num_lines; i++)); do
    #generate random surname and name ,unrandom is the random generator ,tr ... all the characters to include ,fold ... creates random number between 0-9 and add 3 to it,head creates 1 string
    name=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $((3 + RANDOM % 10)) | head -n 1)
    surname=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $((3 + RANDOM % 10)) | head -n 1)

    #select a random party from the party array
    party=${parties[RANDOM % ${#parties[@]}]}

    #write to file
    echo "$name $surname $party" >> "$output_file"
done