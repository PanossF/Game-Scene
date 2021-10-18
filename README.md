# Game-Scene
Game Scene on OpenGL. This project contains important files like Shaders, Libraries and Classes.
Game-Scene.cpp file is the main file for executing the game scene. Different classes and shaders can be found in the folders

All models are free dowloanded on web.

Description: 
A game scene is created with OpenGL representing a random one of an AAA game. The game scene represents a scene of Sekiro Shadows Die Twice which it is not the same as the game, as the included assets are free. The scene contains a female samurai opposed to a warrior in a terrain with a grass texture on it. As for the sky, a night skybox cube-map is selected, and a light positioned very high from the ground covers all over the scene. Another 2 lights are included in the scene implemented with phong-shading, one for each model, so it can add a specular lighting on them, emphasizing the armor. A flower model is included in the scene with many instances. The instancing technique is simple by using a buffer of all the positions of instances and passing it through as a layout in the vertex shader. A camera class has been created for moving and rotating around the scene.
