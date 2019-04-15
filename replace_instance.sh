#!/bin/bash

previous_instance=$(cat ./current_selected_instance)

if [ ! -n "$previous_instance" ]; then
	echo "Previous instance empty"
	exit 1
fi

cp ./gaConfig.txt ./gaConfig.tmp
sed -i "s|$previous_instance|$1|g" ./gaConfig.txt && echo "$1" > ./current_selected_instance

if [ "$?" -eq 0 ]; then
	rm ./gaConfig.tmp
	echo "Successful replace of instance"
	exit 0
else
	mv ./gaConfig.tmp ./gaConfig.txt
	echo "Replacement error"
	exit 1
fi

