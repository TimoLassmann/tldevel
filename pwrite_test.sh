#!/usr/bin/env bash

counter=1

for file in  ./*
do
    if [ -f $file ]; then
	if [[ $file =~ pwtest* ]]; then
	    for file2 in  ./*
	    do
		if [ -f $file2 ]; then
		    if [[ $file2 =~ pwtest* ]]; then
			printname=cmp$counter;
			printf "\t%s\n" $file2;
			if cmp -s <( sort $file  ) <(sort  $file2); then
			    printf "%10s%10s%10s\n"  pwrite $printname SUCCESS;
			else
			    printf "%10s%10s%10s\n"  tomedb $printname FAILED;
			    printf "  - file is different: %s	%s\n" $file  $file2;
			    exit 1;
			fi
			counter=$[$counter +1]


		    fi
		fi
	    done
	fi
    fi
done


