#!/bin/bash

TRIGSETUP="pp200_production_2012"
BADLISTNAME="Issac"

ALLRUNS="runLists/${TRIGSETUP}_ALL.list"
BADRUNS="runLists/${TRIGSETUP}_BAD_$BADLISTNAME.list"
GOODRUNS="runLists/${TRIGSETUP}_GOOD_$BADLISTNAME.list"

# Remove newline characters following commas in All Run Numbers list
all_numbers=$(cat $ALLRUNS | tr -d '[:blank:]' | tr ',' '\n' | awk '!($0 in arr) && NF')

echo "$all_numbers" > $ALLRUNS

# Remove newline characters following commas in Bad Run Numbers list
bad_numbers=$(cat $BADRUNS | tr -d '[:blank:]' | tr ',' '\n' | awk '!($0 in arr) && NF')

echo "$bad_numbers" > $BADRUNS 

# Use grep to remove bad run numbers from the list of all run numbers
grep -vxFf <(echo "$bad_numbers" | sort) <(echo "$all_numbers" | sort) > $GOODRUNS