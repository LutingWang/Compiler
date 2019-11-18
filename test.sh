###############################################
# File Name: test.sh
# Author: Luting Wang
# mail: 2457348692@qq.com
# Created Time: Sun Nov 17 23:59:01 2019
###############################################
#!/bin/zsh

dir=$(dirname $1)
file=$(basename $1)
if [ ! -f $1".std" ]; then
	prep="$1.cpp"
	content=$(sed -e 's/printf[[:space:]]*(\([^;]*\))[[:space:]]*;/\'$'\n''std::cout<<\1<<std::endl;\'$'\n''/g' \
		-e 's/scanf[[:space:]]*(\([^)]*\))[[:space:]]*;/\'$'\n''std::cin>>\1;\'$'\n''/g' \
		$1 | \
	sed -e '/std::cout/s/,/<</g' -e '/std::cin/s/,/>>/g')
	echo "$content" | tr -d "\n" > $prep
	sed -i "" -E -e 's/void[[:space:]]+main/int main/g' -e '1 i\ 
	#include <iostream>
	' $prep
	
	clang++ -o "$1.out" $prep
	cat $dir"/input" | ./$1".out" > $1".std"

	rm $prep $1".out"
fi

cat $dir"/input" | java -jar ./test/Mars.jar ic nc 100000 $1".mips" > $1".out"
tail -n 3 $1".out" > $1".stat"
linenum=`cat $1".out" | wc -l`
last3=`expr $linenum - 2`
sed -i "" "$last3 , $linenum d" $1".out"

diff $1".std" $1".out"
if [ $? -eq 0 ]; then
	echo $file" passed"
fi

