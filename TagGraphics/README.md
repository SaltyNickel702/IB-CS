# Render Documentation
Everything runs off of the `Render` class, which creates a new window and OpenGL context<br>
`Asset` is the superclass of any OpenGL object, used for loading and unloading assets into the OpenGL context<br>
Assets are managed by `Scene`, which contains a list of Assets, which much be manually entered. Assets can be accocciated with multiple scenes. Scenes are included in an `std::map`, and can be swapped using either string IDs or a pointer using `Render::setScene(<arg>)`<br>

## Types of Assets
- `TickFunc`
	- [Copy from Render.h]
- `Shader`
	- [Copy from Render.h]
- `Texture`
	- [Copy from Render.h]
- `Mesh`
	- [Copy from Render.h]
	- Render calls are controlled soly by the programmer using `TickFunc` instances

## Basics to draw calls
Create new `Scene` instance and add all Assets to it as they're created<br>
Create a `TickFunc` with `.permanent` set to true <br>
Inside the TickFunc, put the draw call<br>

Choose your shader with `glUseProgram([shader ID])`

Uniforms are added with 
```cpp
glUniform[uniform type](glGetUniformLocation([shader ID], [uniform name]), ...[values])
```

Textures are added with: <br>
```cpp
glActiveTexture(GL_TEXTURE0 + i);
glBindTexture(GL_TEXTURE_2D, [texture data]);
glUniform1i(glGetUniformLocation([shader ID], [texture name]), i)
```
Where i is the nth texture added to the shader. While buffered as 1 int, the uniform type inside the shader is `Sampler2d`

Final draw call is:
```cpp
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES,[indice count],GL_UNSIGNED_INT, 0);
glBindVertexArray(0); //unbind the VAO
```

# Requirements
## Toolsets
Doesn't really matter toolset source, but requires:
- GLFW
- GLM
### Windows (MSYS2 UCRT64):
pacman -S mingw-w64-ucrt-x86_64-glfw <br>
pacman -S mingw-w64-ucrt-x86_64-glm

## Other Requirements
C++23<br>
OpenGL 4.1 (MacOS is depreciated passed 4.1)