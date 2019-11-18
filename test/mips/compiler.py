import re
import sys


assert len(sys.argv) == 3
f = open(sys.argv[1])
file = f.read()
file = re.sub('printf\s*\(\s*("[^"]*")\s*,\s*([^;]*)\s*\)\s*;', 
              lambda m: "std::cout<<" + m.group(1) + "<<" + m.group(2) + "<<std::endl;", file)
file = re.sub('printf\s*\(\s*("[^"]*")\s*\)\s*;', 
              lambda m: "std::cout<<" + m.group(1) + "<<std::endl;", file)
file = re.sub('printf\s*\(\s*([^;]*)\s*\)\s*;', 
              lambda m: "std::cout<<" + m.group(1) + "<<std::endl;", file)
file = re.sub('scanf\s*\(\s*(.*?)\s*\)\s*;', 
              lambda m: "std::cin>>" + ">>".join(m.group(1).split(',')) + ";", file)
file = re.sub("void\s+main\s*\(\s*\)\s*(\{.*\})", 
        lambda m: "int main()" + re.sub("return", "return 0", m.group(1)), file, flags=re.S)
f.close()

f = open(sys.argv[2], mode='w')
f.write("#include <iostream>\n\n")
f.write(file)
f.close()
