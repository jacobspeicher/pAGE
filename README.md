# pAGE
paetramon's Autogeneous Game Engine

### Current Progress
Can add a 2D triangle and 3D cube to the scene, move the camera, and adjust the objects transforms.
![Gif of cube and triangle creation, adjusting the size of the cube, and moving the camera around the scene](gifs/BasicFunctionality.gif)

Can use the mouse to select objects in the scene, and hold the right mouse button while hovering over the scene to move the camera.
![Gif of selecting objects in the scene using the mouse, then holding the right mouse button to allow camera movement within the scene](gifs/ClickToSelect_Mouse_andMove.gif)
^ This gif was created when I was using the Moller-Trumbore algorithm for ray/triangle intersection. This method worked well when I was just drawing cubes with 12 triangles, or triangles with well, 1 triangle. However, performance became an issue when I started importing more complex models. The Moller-Trumbore algorithm code is still in there, but now I handle selection by using an extra color attachment on the OpenGL framebuffer, where each object is drawn with a color based on the object ID. Then a simple pixel color check handles determining which object to select.
![Gif showing alternating color attachments of the OpenGL framebuffer, as the user clicks between different objects. The first color attachment shows the models normally, then the second color attachment shows the models as very slightly different shades of black. The colors of the second attachment are based on the object ID to help determining selection.](gifs/ColorBasedSelection.gif)

Supports rudimentary lighting with a single directional light source. Point and Spotlight sources will be added, as well as being able to support multiple lights at once.
![Gif of a light source being modified to show the light moving along the surface of 5 objects (a cube, sphere, cone, cylinder, and backpack model). The light source is further modified to change the color and strength of the ambient and diffuse properties](gifs/DirectionalLighting.gif)