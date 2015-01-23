main: build copy-libs

clean:
	@echo "################ clean ################"
	ndk-build --directory=./src clean

build:
	@echo "################ build ################"
	ndk-build --directory=./src

rebuild: clean build copy-libs

copy-libs:
	@echo "################ copy-libs ################"
	cp -vf ./src/obj/local/armeabi/*.a ./lib