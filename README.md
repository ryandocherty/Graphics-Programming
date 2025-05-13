# Graphics Programming: 3D Rendering Simulation

## Description

This project demonstrates fundamental graphics rendering concepts using C++ with OpenGL and GLEW. The primary goal was to simulate how rendering works, including the use of shaders, object culling, and texturing. The application implements a basic camera system that allows for rotation and zooming to view various 3D objects in the scene.

## Features

- **3D Objects**: Rendered cubes with different textures, including:
  - Minecraft block texture
  - Dice texture
  - Rubik's cube texture

- **Camera System**:
  - Ability to rotate and zoom to explore the scene.
  - Implemented various camera controls to navigate around the objects.

- **Shaders**: Utilized vertex and fragment shaders to manage the graphics rendering pipeline.

- **Object Culling**: Implemented frustum culling to optimize rendering performance by discarding objects outside the camera's field of view.
