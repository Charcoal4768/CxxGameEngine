This is a simple C++ game engine. I made it as an educational project to teach myself how rendering pipelines, object collisions, physics and various mini-systems behind real game engines actually work.
This repository includes an asteroid game made with the engine. It has healthbars floating above damageable entities and a fun gameplay loop.
The engine follows a building-blocks style achritecture where complex objects and UI elements can be created by mixing severall different base structs

To complie this project, please create a libs/ folder in the directory where the CMake file is (main directory) and add the 64 bit development version of these libraries:
- SDL2
- SDL2_image
- SDL2_ttf

Current status: Rendering pipeline and UI systems are fully functional. Physics system is undergoing a refactor from AABB to Circle-based collision to better handle rotated entities.