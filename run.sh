counter=0
for ((i=$1;i<=$2;i++)) ; do
    ../x64/Release/ttp_ga.exe $i &
    pids[$counter]=$!
    counter=$((counter + 1))
done

for pid in ${pids[*]}; do
    wait $pid
done
echo "done"

