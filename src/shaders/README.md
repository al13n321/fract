Directory with editable code and "configuration" files.
Code structure is largely inspired by Fragmentarium.
There's no GUI, but there's instant runtime reloading and recompilation of all
relevant shader and config files as soon as they change.

conf.json - The entry point for shader and parameter lookup.
						The only required fields are "raytracer" and "renderer" - 
						relative paths to shaders.
Predefined - Not supposed to be modified.
Common - Structures and entry points for raytracer and renderer.
Raytracers - Generic raytracer/raymarcher implementations that can work with
             arbitrary distance estimate functions.
Renderers - Renderer implementations. Take raytracer output and produce
            pixel color. Lighting, AO, coloring etc. belongs here.
Examples - Some working DE's.
<Your cool stuff> - Whatever awesomeness you can come up with!
