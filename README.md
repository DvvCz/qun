# trender

This is an attempt to make a relatively simple renderer using modern OpenGL (4.5) and C++ (23)

## Features

- GLSL Shader Hotreloading
- Dynamic entity manipulation making use of [EnTT](https://github.com/skypjack/entt)
- Rendering .obj assets with meshes and multiple materials

## Sample

![sample](./sample.png)

## Troubleshooting

| Error | Solution |
| --- | --- |
| `libdecor-gtk-WARNING: Failed to initialize GTK.` | Either run the program outside VSCode or force VSCode to use Wayland (`code --enable-features=UseOzonePlatform --ozone-platform=wayland`) |
