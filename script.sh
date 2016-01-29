#/bin/bash

directory=build-RNAcomp-Desktop-Debug
options=-'h -g -r ./data/MirbaseFile.fa -i ./data/GSM518430-13540.fa --all'
#options=-'h -g -r ./data/sativa.fa -i ./data/thaliana.fa --all'

cmd_time=/usr/bin/time
opt_time='-v -o'

for i in {1..1}
do
	echo $i
	$cmd_time $opt_time ./out/time_$i ./$directory/RNAcomp $options 1>./out/logs_$i
done
