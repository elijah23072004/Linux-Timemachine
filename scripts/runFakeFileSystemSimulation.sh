location="/home/eli/Documents/backupsTest/fileSystem"
output1="/home/eli/Documents/backupsTest/noCompression"
output2="/home/eli/Documents/backupsTest/compression"
flags1="-r 50"
flags2="-cr 50"
changeAmount="25000000"

mkdir ${output1}
mkdir ${output2}

dataLocation="/home/eli/Documents/backupsTest/data.dat"
rm ${dataLocation}
touch ${dataLocation}
./build/randomFileSystemGenerator ${location}
for i in {0..75}
do
    echo ${i}
    ./build/createRandomDifferences ${location} ${changeAmount}

    ./build/linuxTimeMachineBackend ${location} ${output1} ${flags1}&
    ./build/linuxTimeMachineBackend ${location} ${output2} ${flags2}&
    sum=$(du -sb ${location} | cut -f1)
    echo ${sum} >> ${dataLocation}
    wait $(jobs -p)

done
