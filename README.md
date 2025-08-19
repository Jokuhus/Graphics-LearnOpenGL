# SCOP
This project is to study OpenGL.
The program reads OBJ files and renders 3D objects.

# Commands
make - will compile program using g++/clang++ and OpenGL library with GLFW to control window.
make clean - remove object files.
make fclean - remove all object files and the program which is compiled.

./app path_to_obj_file_1 path_to_obj_file_2 ...
- program run command.

q,e / w,s / a,d - rotate by object's axis.
arrows - translate by x & y axis of camera view.
z,x - translate by z axis of camera view.
c - change object focus
m - change object texture mode.
r - return object to original rotation.
t - return object to original location.
f, l - change OpenGL state to either line mode or fill mode.
