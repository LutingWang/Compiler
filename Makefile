./build/compiler: ./CMakeLists.txt ./**/CMakeLists.txt
	cd ./build && \
	cmake .. > log.txt

.PHONY: run clean xcode xclean
	
run:
	cd ./build && make clean
	make -C ./build >> ./build/log.txt
	./build/compiler

clean:
	rm -r ./build/*

xcode:
	cd ./Xcode && cmake -G "Xcode" ..

xclean:
	rm -r ./Xcode/*
