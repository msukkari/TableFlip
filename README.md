#TableFlip


TableFlip is a 3D table flipping simulator I made in my free time. The main purpose of this project was to challenge my knowledge of graphics programming (using OpenGL), as well as to get some hands-on experience with collision detection and physics.


#Graphics

The graphics for this project were done using the OpenGL API. GLSL shaders were used to simulate real-world lighting, which can be best seen by the specular lighting in the metal texture. All of the models in this simulation are of the file type ".obj", and were loaded using the Assimp library. As for the textures, I used SOIL (Simple OpenGL Image Library), to load the various texture files.

#Physics

A 3D table flipping simulator would be pointless without some real-world physics and collision. To do this, I used the Bullet physics engine. This was probably the most difficult part of this simulation and as a result, has the most bugs. For the most part however, the collision works very well and the simple 3D model of the table allows for an almost 1:1 match for the collision body.

#Sound

Game sound is handled using the SFML library. The source of these sound clips is the result of googling "free drum sound clips".

#Controls

In order to flip a table, the player walks up to the table and presses 'E'. The player can also jump using 'space'. Originally, my friend pointed out a bug where the player could jump multiple times. It was not too hard to fix, however I ended up leaving the bug in since it's fun to jump around.