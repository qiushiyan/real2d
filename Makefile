.PHONY: build Sclean

build:
	cmake --build build

clean:
	rm -rf build/*