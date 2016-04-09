# RoboticsProject4
Skynet



#### TODO:

  Build lights at runtime by user input
  
  Read lights from file
  
  Read robots from file
  
  Make sure robot's brains are processing correctly.
  
#### Extra TODO:

  Try different lighting algorithms in Light.frag just to see what happens
  
  Move stuff out of main.cpp
  
#### Extra Extra TODO:

  (Conor) Figure out how to properly do shadows even though they aren't required


# Tips

  The 'opengl_files' folder now has everything that is required to get openGl running. Remove the opengl-files changes from the previous project because they might interfer. http://aschultz.us/blog/archives/176
  
  New stuff: (All of these folders and files will be inside the opengl_files folder)
  
  Configuration Properties -> C/C++ -> General -> Additional Include Directories.....Select the 'include' folder for glut and glew (separately, so you will have two entries).
  
  Configuration Properties -> Linker -> General -> Additional Library Directories.....Select the 'lib' file for glut and glew (separately, so you will have two entries)
  
  Configuration Properties -> Linker -> Input -> Additional Dependencies......add 'glew32.lib' and 'glut32.lib' on separate lines.
  
  The process is similar to this, but with different files, because we aren't using freeglut: https://www.youtube.com/watch?v=0CQP8huwLCg
