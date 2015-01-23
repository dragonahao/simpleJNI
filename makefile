main: build copy-libs

clean:
	@echo "################ clean ################"
	ndk-build clean

build:
	@echo "################ build ################"
	ndk-build

rebuild: clean build copy-libs

copy-libs:
	@echo "################ copy-libs ################"
	cp -vf ./obj/local/armeabi/*.a ./lib