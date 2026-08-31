# OpenGL Racing Track Renderer

A real-time 3D racing game built from scratch in C++ and OpenGL, submitted as coursework for IN3005 Graphics at City, University of London. It renders a Catmull-Rom spline race track with a drivable car, dynamic lighting and shadows, water, procedural vegetation, particle effects and a full HDR bloom post-processing pipeline.

Built on top of the department-provided OpenGL template (skybox, terrain, camera, basic shaders), extended with the systems listed below.

## Features

- HDR bloom: multi-pass FBO pipeline (bright-pass filter + blur + composite)
- Shadow mapping with percentage-closer filtering (PCF)
- Catmull-Rom spline track generation with offset curves for track boundaries and collision
- Procedural placement of trees, grass and speed-boost pickups along the track
- Particle systems for dirt kick-up, leaves and speed lines, using alpha blending
- Frustum culling to skip rendering off-screen geometry
- Reflective water pass with depth masking
- Positional audio via FMOD (engine, wind, boost, crash, collectible sounds)
- HUD text rendering (FPS, speed, lap count) via FreeType
- Skybox, Assimp-loaded meshes (car, horse, barrel), and texture-mapped terrain

## Controls

| Key | Action |
| --- | --- |
| `W` / `S` | Accelerate / brake |
| `A` / `D` | Steer left / right |
| `1` – `4` | Switch camera mode |
| `F1` | Play event sound |
| `Esc` | Quit |

## Building

Requires Windows and Visual Studio 2019+ (v142 toolset) with the "Desktop development with C++" workload. All third-party dependencies (Assimp, FreeType, FreeImage, FMOD, GLEW, GLM) are vendored under `OpenGLTemplate/OpenGLTemplate/include` and `lib`, so no package manager setup is needed.

1. Open `OpenGLTemplate/OpenGLTemplate.sln`
2. Select `Release` or `Debug`, platform `x64`
3. Build and run (F5) — resources and shaders are loaded relative to the project directory, and `OpenGLTemplate/OpenGLTemplate/bin` is added to the debugger `PATH` so the runtime DLLs resolve automatically

## Project layout

```
OpenGLTemplate/OpenGLTemplate/
  *.cpp, *.h        game, rendering and math source
  resources/         shaders, models, textures, skyboxes, audio
  bin/               runtime DLLs (Assimp, FreeImage, FMOD, GLEW)
  include/, lib/     vendored third-party headers and import libraries
```

## Report

`IN3005 Abhijit Shidlagatta Graphics Coursework Report.pdf` documents the rendering techniques used and the implementation decisions behind them.
