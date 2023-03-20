#  Project Details
Course: CAP5705-3789(11514) – CG
Name: Sheela Rani Ippili, Ben Rheault

## Introduction

This project aims at creating an environment called "GatorLand" by implementing different Graphics techniques using C++, OpenGL and Maya.

## Prerequisites
Environment: MacOS, Bigsur 11.5.2 
XCode: Version 12.5.1

## Frameworks Used
- OpenGL
- GLEW
- GLFW
- GLM

## Executing the assignment
- There is one C++ file called **TheSwamp.cpp** which has all the functionalities implemented.
- Run the below command and add the path of Assignment3 folder to direct to the home directory.
```
cd <ASSIGNMENT3_FOLDERPATH> 
```
- Open terminal, run the below command to compile the TheSwamp.cpp program. Remove ```-framework``` if not executing the program on mac.
```
g++ -I/opt/homebrew/Cellar/glfw/3.3.4/include/ -I/opt/homebrew/Cellar/glew/2.2.0_1/include/ -I/opt/homebrew/Cellar/glm/0.9.9.8/include/ -L/opt/homebrew/Cellar/glfw/3.3.4/lib/ -L/opt/homebrew/Cellar/glew/2.2.0_1/lib/ -L/opt/homebrew/Cellar/glm/0.9.9.8/lib/ -lglfw -lglew  -framework OpenGL TheSwamp.cpp
```
- run the command ```./a.out``` to run the program.

- Enter ```W, S, A or D ``` keys to go front, back, left and right in the scene. Use mouse to change the direction of the camera

- Click on the ```esc``` button to close the output window.

## Troubleshooting

- If the above commands do not compile the program as expected, locate the GLM, GLEW and GLFW libraries by running the below commands in terminal and replace the respective paths in the above commands. The desired path would be of the below form
```
  /opt/homebrew/Cellar/glm/0.9.9.8
```

- Commands to run to locate glfw, glew and glm libraries.
```
brew info glm
```
```
brew info glew
```
```
brew info glfw
```
- Add the above located paths in the below command to compile main.cpp or bonus.cpp
```
g++ <GLFW_INCLUDE_PATH> <GLEW_INCLUDE_PATH> <GLM_INCLUDE_PATH> <GLFW_LIB_PATH> <GLEW_LIB_PATH> <GLM_LIB_PATH> -lglfw -lglew -framework OpenGL <PROGRAM_NAME.cpp>
```

