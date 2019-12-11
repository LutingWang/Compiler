###############################################
# File Name: test.sh
# Author: Luting Wang
# mail: 2457348692@qq.com
# Created Time: Sun Nov 17 23:59:01 2019
###############################################
#!/bin/zsh

function test_case() {
	dir="./"$1
	file=${dir}"/"$1
	if [ ! -f ${file}".std" ]; then
		python3 compiler.py ${file} ${file}".cpp"
		clang++ -o ${file}".exe" ${file}".cpp" -w
		cat ${dir}"/input" | ./${file}".exe" > ${file}".std"
	fi
	
	cat $dir"/input" | java -jar ./mars.jar ic nc 10000000 ${file}".mips" > ${file}".out"
	tail -n 3 ${file}".out" > ${file}".stat"
	linenum=`cat ${file}".out" | wc -l`
	last3=`expr $linenum - 2`
	sed -i "" "$last3 , $linenum d" ${file}".out"
	
	diff ${file}".std" ${file}".out"
	if [ $? -eq 0 ]; then
		echo $1" passed"
	fi
}

this=$(dirname $0)
cd $this

if [ $# -eq 1 ]; then
	test_case $1
else
	for tname in `ls`
	do
		if [ ! -d $tname ]; then
			continue
		fi
		test_case $tname
	done
fi
