# SCOP
This project is to study OpenGL.
The program reads OBJ files and renders 3D objects.

# Commands
__make__ - will compile program using g++/clang++ and OpenGL library with GLFW to control window.

__make clean__ - remove object files.

__make fclean__ - remove all object files and the program which is compiled.

----------------------------------------------------------------------------------------------------
- program run command
  
	> ./app path_to_obj_file_1 path_to_obj_file_2 ...

- __q__, __e__ / __w__, __s__ / __a__, __d__ - rotate by object's axis.
- __arrows__ - translate by x & y axis of camera view.
- __z__, __x__ - translate by z axis of camera view.
- __c__ - change object focus.
- __m__ - change object texture mode.
- __r__ - return object to original rotation.
- __t__ - return object to original location.
- __f__, __l__ - change OpenGL state to either line mode or fill mode.
