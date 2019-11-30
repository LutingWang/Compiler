###############################################
# File Name: test.sh
# Author: Luting Wang
# mail: 2457348692@qq.com
# Created Time: Sat Nov 30 13:55:45 2019
###############################################
#!/bin/zsh

this=$(dirname $0)
cd $this

testbase="./test/"$1
for testname in `ls $testbase`
do
	if [ -d $testbase/$testname ]; then
		echo "\ncase: "$testname
		./build/compiler $testbase/$testname/$testname
	fi
done
