#!/bin/bash

FILE=""
CMD=""
PID=""
SORT=false

function usage() {
    echo "USAGE:"
    echo "  ./handle-ps.sh -f <filename> [-s] [-q <CMD>] [-p <PID>]"
    echo "OPTIONS:"
    echo "  -f: input file"
    echo "  -s: sort by priority and pid"
    echo "  -q: query processes with queried command"
    echo "  -p: get process's ancestors"
}

while getopts "f:q:p:sh" opt; do
    case $opt in
        f) FILE=${OPTARG} ;;
        q) CMD=${OPTARG} ;;
        p) PID=${OPTARG} ;;
        s) SORT=true ;;
        h) usage; exit 0 ;;
        ?) usage; exit 1 ;;
    esac
done

if [ -z $FILE ]; then
    usage
    exit 1
fi

# You can remove ":" after finishing.
if $SORT; then
    # Your code here. (1/3)
    sort -k4nr -k2n ${FILE}
elif [ ! -z "$CMD" ]; then
    # Your code here. (2/3)
    grep "${CMD}" "${FILE}"
elif [ ! -z $PID ]; then
    # Your code here. (3/3)
   # while [ $PID -ne 0 ];
   # do
   # PID=`awk -v input=$PID  '$2==input {print $3}' ${FILE}`
   # echo ${PID}
#done
   son=$PID
   while [ $son -ne 0 ]
	   do
		   grep ${son} ${FILE} | awk -v input=$son "$2==input {print $3; son=$3}"
		   done

else
    usage
    exit 1
fi
