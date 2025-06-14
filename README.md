# Spaceship Demo

Small OpenGL project showing a simple spaceship taking off in an arc with particles simulating the rocket's exhaust. The terrain is based on an area in Finland, its `.obj` and license is found in `assets`.

The project was mainly used to learn OpenGL early on, transformations and matrices are implemented by scratch and uses fontstash to do text rendering.

#### Keybinds

- `Space` - Enable / disable cursor focus
- `R` - Recompile and reload shaders
- `F` - Start rocket animation
- `R` - Reset rocket animation
- `V` - Toggle split-screen
- `C` - Cycle through camera states
- `Shift + C` - Cycle through second screen camera states
- `Shift` - When held, increase camera fly speed
- `Ctrl` - When held, decrease camera fly speed

## Usage

- Clone repo
- Run `premake5 vs2022` in terminal
- Open `.sln` file
- Compile and run from run configurations