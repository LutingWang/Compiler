###############################################
# File Name: judge.sh
# Author: Luting Wang
# mail: 2457348692@qq.com
# Created Time: Wed Nov 13 11:25:11 2019
###############################################
#!/bin/zsh

function copy_dir() {
	for file in `ls $1` 
	do
		if [ -d $1"/"$file ] 
		then
			copy_dir $1"/"$file
		else
			cp $1"/"$file `pwd`"/judge/"
		fi
	done
}

rm -r ./judge/*
copy_dir ./include
copy_dir ./src
rm ./judge/CMakeLists.txt

for file in `ls ./judge/*.cpp ./judge/*.h`
do
	cat $file | sed 's/".*\/include\/\(.*\)"/"\1"/g' >> $file
done

zip ./judge/Archive.zip ./judge/*
