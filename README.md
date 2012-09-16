About
=====

This experiment was written to validate a claim that reversed depth rendering 
results in improved precision even when used with D24S8 depth buffer.
Two quads are rendered on top of each other with 1 unit gap between them.
Coloured quad on top of a black one. They are stretched 50k units forward.
Camera near clip distance is 0.5, far is 50k.
White line markers are drawn every 1k units
Trivial vertex shader is used: out_pos = mul(float4(in_pos,1), mat_viewproj);
 
Controls
========

Hold left mouse button to look around.
Use WASD+EQ keys to move.
Ctrl to slow-down, Shift to speed-up.
H to hide or show help text.

How to compile
==============

You will need Visual Studio and DirectX SDK installed.
Run premake.exe to generate Visual Studio solution.
Use generated solution from 'build' directory.
