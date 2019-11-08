./build/compiler: ./CMakeLists.txt ./**/CMakeLists.txt
	cd ./build && \
	cmake .. > log.txt

.PHONY: run clean xcode judge
	
rerun:
	cd ./build && make clean
	make -C ./build >> ./build/log.txt
	./build/compiler

run:
	make -C ./build >> ./build/log.txt
	./build/compiler

clean:
	-rm -r ./build/* ./include/compilerConfig.h ./judge/* ./Xcode/*

xcode:
	cd ./Xcode && cmake -G "Xcode" ..

judge:
	rm ./judge/*
	-cp ./src/* ./judge/
	cp ./src/**/* ./judge/
	cp ./include/* ./judge/
	rm ./judge/CMakeLists.txt
	zip ./judge/Archive.zip ./judge/*
