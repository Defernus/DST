# DST
DST Shader Toy

## Build
Install g++.  
```sudo apt install g++```  
  
Install glfw.  
```sudo apt install libglfw-dev```  
  
Download [glad](https://glad.dav1d.de/) (select gl Version 4.5 and only checkbox "Local Files").  
Then compile it.  
```g++ -c glad.c ```  
and locate glad headers at /usr/include  

Build shadertoy!  
```./build.sh```  

## Run
```sudo chmod +x ./bin/out```  
```./bin/out```  