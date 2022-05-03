Actor Hierarchy Viewer
----------------------

A simple program to view an actor hierarchy that has been saved
using BrActorSave. It is a very basic program intended purely as an
illustration of how to read in simple data. The saved hierararchy contains
lights they may be turned on using command line arguments and if the
scene contains a camera one may be selected as the viewpoint in a similar
way.

Once a model is loaded use the mouse to rotate the view camera and
left and right mouse buttons to zoom in and out respectively.

Use command line arguments to load in parts of the scene as follows:

-p <palette>	Load palette
-h <table>	Load shade table
-t <texture>	Load texture pixelmap
-s <material>	Load material
-m <model>	Load model
-a <actor>	Load actor
-c "camera"	Select camera: "camera" is the camera actor identifier
-l "light"	Turn on light: "light" is the light actor identifier

Command line arguments may be preceded by @<filename> to indicate that the
contents of the specified file should be used as arguments.
 
For example

brview @file with file containing the following:

-p example.pal
-h example.tab
-t example.pix
-t example2.pix
-s example.mat
-s example2.mat
-m venus.dat
-m torus.dat
-a example.act
-c camera
-l light

Important note
--------------

All of the data read in by brview must have correct identifier information
including the pixelmaps and models.
