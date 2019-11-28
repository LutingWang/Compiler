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

cur_dir=$(dirname $0)
jud_dir=${cur_dir}/judge

rm -r ${jud_dir}/*
copy_dir ${cur_dir}/include
copy_dir ${cur_dir}/src
rm ${jud_dir}/CMakeLists.txt

for file in `ls ${jud_dir}/*.cpp ${jud_dir}/*.h`
do
	sed -i "" 's/#include ".*\/\([^\/]*\)"/#include "\1"/g' $file
done

cp ${cur_dir}/test/mips/integral/integral ${jud_dir}/testfile.txt
cd ${jud_dir}
zip Archive.zip ./*
clang++ *.cpp *.h -std=c++11
