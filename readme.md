# Simple ECS game engine

level 1

![](https://github.com/qiushiyan/images/blob/63fc463ba10d801b45556778de509527311d0f7b/real2d/level1.png?raw=true)

level 2

![](https://github.com/qiushiyan/images/blob/63fc463ba10d801b45556778de509527311d0f7b/real2d/level2.png?raw=true)

# Usage

```
cmake -B build
cmake --build build
```

If on Mac use the executable `./main` directly.

I also had to create a sym link after some Apple update:

```
sudo ln -s /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk
```