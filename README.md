# FLIGHT

<p align="center">
  <img src="screenshot.png"/>
</p>

Flight is an upcoming open source aerial combat game built on OpenGL. It features procedurally generated content, and smooth cel shaded graphics.

## Building

If you're running macOS everything should go fairly smoothly. Otherwise, you may need to add a few lines to the CMake script in build/ according to your platform (this would be a great way to contribute).

```bash
cmake deps/SFML-2.4.1/ && make -C deps/SFML-2.4.1/ && make install
cmake deps/libnoise/ && make -C deps/libnoise/ && make install
cmake build/ && make -C build/
```
