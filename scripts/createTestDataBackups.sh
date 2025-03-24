#!/bin/bash

location="/home/eli/Documents/External/sampleData"
output1="/home/eli/Documents/External/sampleData/backups/lowBackupCompression"
output2="/home/eli/Documents/External/sampleData/backups/medBackupCompression" 
output3="/home/eli/Documents/External/sampleData/backups/highBackupCompression" 
output4="/home/eli/Documents/External/sampleData/backups/lowBackup" 
output5="/home/eli/Documents/External/sampleData/backups/medBackup" 
output6="/home/eli/Documents/External/sampleData/backups/highBackup"
echo $outputs


flags1="-cr 50"
flags2="-r 50"
backups=("/start")
mkdir "/home/eli/Documents/External/sampleData/backups"
mkdir ${output1}
mkdir ${output2}
mkdir ${output3}
mkdir ${output4}
mkdir ${output5}
mkdir ${output6}

./build/linuxTimeMachineBackend ${location}"/low/start" ${output1} ${flags1}
./build/linuxTimeMachineBackend ${location}"/med/start" ${output2} ${flags1}
./build/linuxTimeMachineBackend ${location}"/high/start" ${output3} ${flags1}
./build/linuxTimeMachineBackend ${location}"/low/start" ${output4} ${flags2}
./build/linuxTimeMachineBackend ${location}"/med/start" ${output5} ${flags2}
./build/linuxTimeMachineBackend ${location}$/"high/start" ${output6} ${flags2}
for i in {0..25}

do 
    echo ${i}
    echo ${location}${i}
    echo ${output}
    ./build/linuxTimeMachineBackend ${location}"/low/"${i} ${output1} ${flags1}
    ./build/linuxTimeMachineBackend ${location}"/med/"${i} ${output2} ${flags1}
    ./build/linuxTimeMachineBackend ${location}"/high/"${i} ${output3} ${flags1}
    ./build/linuxTimeMachineBackend ${location}"/low/"${i} ${output4} ${flags2}
    ./build/linuxTimeMachineBackend ${location}"/med/"${i} ${output5} ${flags2}
    ./build/linuxTimeMachineBackend ${location}$/"high/"${i} ${output6} ${flags2}
    
done 



