#!/bin/bash

TRIGSETUP="AuAu_200_production_low_2014"

# Remove newline characters following commas in All Run Numbers list
all_numbers=$(cat ${TRIGSETUP}_ALL.list | tr -d '[:blank:]' | tr ',' '\n' | awk '!($0 in arr) && NF')

echo "$all_numbers" > ${TRIGSETUP}_ALL.list 

# Remove newline characters following commas in Bad Run Numbers list
bad_numbers=$(cat ${TRIGSETUP}_BAD.list | tr -d '[:blank:]' | tr ',' '\n' | awk '!($0 in arr) && NF')

echo "$bad_numbers" > ${TRIGSETUP}_BAD.list 

# Use grep to remove bad run numbers from the list of all run numbers
grep -vxFf <(echo "$bad_numbers" | sort) <(echo "$all_numbers" | sort) > ${TRIGSETUP}_GOOD.list 