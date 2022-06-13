
.PHONY: build clean

build:
	cmake --build build

clean:
	rm -rf build/*