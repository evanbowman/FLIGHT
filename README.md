# FLIGHT

<p align="center">
  <img src="screenshot.png"/>
</p>

Flight is an ~~upcoming~~<sup>1</sup> open source aerial combat game built on OpenGL. It features procedurally generated content, and smooth cel shaded graphics.

<sup>1</sup> I was developing the game on a mac, and Apple is dropping OpenGL, so I've decided not to continue this project.

## Building

If you're running macOS everything should go fairly smoothly. Otherwise, you may need to add a few lines to the CMake script in build/ according to your platform (this would be a great way to contribute).

```bash
cd build/
cmake .
cd ../deps/
# Compile each dependency
cd ../build/
make
```

Note: I believe there's a race-condition on the linux builds for which I need to add some more locks. I saw the error where XLib complains that you didn't call XInitThreads, which is never the real problem, almost always a synchronization thing.
