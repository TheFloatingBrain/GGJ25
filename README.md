# GGJ25

https://github.com/user-attachments/assets/ed908938-150c-48a2-bf76-ada249217056

![Screenshot From 2025-01-26 22-15-29](https://github.com/user-attachments/assets/900d10d3-c40f-4390-b4a2-c33473fafc7d)


# What is it?
 - Mini 3D Game engine with Physics
 - Physics Demo
 - Character Demo

# How To Build

## Linux
Clone the repo, then
```
git clone ...
cd GGJ25
mkdir build
cd build
cmake ../
make -j$(nproc)
```

Move or otherwise make availible the assets directory in the same directory OR make a symbolic link 

```
ln -s assets ../assets
```

ensure assets is accable from the same directory as the executable then

```
.\character_scratchpad
```
or

```
.\physics_scratchpad
```



## Windows
Clone the repo, then

```
git clone ...
cd GGJ25
mkdir build
cd build
cmake ../
msbuild ALL_BUILD.vcxproj
```

Move or otherwise make availible the assets directory in the same directory as the executable then

```
.\character_scratchpad
```
or
```
.\physics_scratchpad
```


