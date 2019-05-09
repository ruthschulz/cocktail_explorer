#Cocktail Explorer

This was an interface developed in 2003 for my engineering honours thesis at the University of Queensland under the supervision of Dr Gordon Wyeth. It was a joint project with Polly Alexander, who built the cocktail maker.

The 2003 code has been refactored, with updated data structures and classes, using Visual Studio 2017, FreeGLUT, and GLEW.

I wanted some practice with coding in C++, so set myself a project to update this interface. So this is an interesting collaboration between me in 2003 and me in 2019. 2003-me made some interesting choices, but generally had logical and sufficiently commented code. This is a nice interface for exploring different cocktails, and I still have some ideas for how it could be improved and made more useful.

##From the thesis and poster

PipeDream is an automated cocktail maker that has been developed during 2003. 
A cocktail is ordered through the interface, and the machine mixes the drink. 
Three interfaces were designed - a list based interface, a dynamic interface and a combined interface. This thesis also includes the development of a management interface and communications with the machine concerning the cocktails that will be made. 
A parallel thesis was undertaken to develop the cocktail dispensing part of the Automated Cocktail Maker that is capable of dispensing and mixing quality cocktails.

###Users of the interface will be able to:
- Search for a drink by name
- Search for a drink by ingredients
- Search for a drink by similarity to other drinks
- Design their own drink

A video of the original version of the interface is available at:
https://www.youtube.com/watch?v=y9IVzYFmmIg

##Drinks and ingredients files

These files store the information used by the interfaces, including a database of drinks and possible ingredients, and information on the ingredients available in the cocktail maker.

- drinks.txt
- ingredients.txt
- ingredients2.txt
- pipeingredients.txt
- pipeamounts.txt

##Project Set-up

To set up the project, I found the following tutorial useful:
http://in2gpu.com/2014/10/15/setting-up-opengl-with-visual-studio/

- Open Visual Studio 2017, create new Visual C++ empty project
- Add existing item to project - main.cpp
- Set up FreeGLUT and GLEW (download and extract both, also compile FreeGLUT)
- Create new folder Dependencies in project folder
- Create new folder glew in Dependencies
- Add eglew.h, glew.h, glew32.lib, glxew.h, and wglew.h to this folder
- Create new folder freeglut in Dependencies
- Add freeglut.h, freeglut_ext.h, freeglut_staticd.lib, freeglut_std.h, freeglutd.exp, freeglutd.lib, and glut.h to this folder
- In Visual Studio, open Project-Properties
- In Linker-General-Additional Library Directories, add Dependencies\freeglut;Dependencies\glew;
- In Linker-Input-Additional Dependencies, add opengl32.lib;freeglutd.lib;glew32.lib;
- Add freeglutd.dll and flew32.dll to Debug directory

##License:

MIT License