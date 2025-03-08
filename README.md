
# mandelbrot

A C99 program that visualizes the Mandelbrot set and allows interactive exploration using keyboard controls. Users can navigate through the complex plane, zoom in and out as well as save/load views. The program supports both command-line configuration and JSON settings files for flexibility.

## Features

-   **Real-time Navigation**: Move through the Mandelbrot set using key presses.
    
-   **Smooth Zooming**: Zoom in and out for deep exploration of fractal details.
    
-   **High-Precision Computation**: Uses GMP for arbitrary-precision calculations.
    
-   **Parallelized Rendering**: Optimized with OpenMP for better performance.
    
-   **Configurable Settings**: Modify parameters via command-line arguments or a JSON settings file.
    
-   **Save and Load Views**: Store and restore specific views of the Mandelbrot set.
    

## Dependencies

This project relies on the following external libraries:

-   [SDL](https://github.com/libsdl-org/SDL) - Visualization and rendering.
    
-   [cJSON](https://github.com/DaveGamble/cJSON) - JSON-based settings and persistence.
    
-   [GMP](https://gmplib.org/) - Arbitrary-precision arithmetic.
    
-   [OpenMP](https://www.openmp.org/) - Multi-threaded parallelization.
    

## Installation

### Prerequisites

Ensure you have the required libraries and build systems installed on your system (OpenMP comes automatically with GCC but is required for Clang):

#### Arch-based systems:

```
sudo pacman -S sdl2-compat gmp cjson [clang openmp] [cmake]
```

#### Debian-based systems:

```
sudo apt install libsdl2-dev libgmp-dev libcjson-dev [clang libomp-dev] [cmake]
```

#### MacOS:

TBA

#### Windows:

lol, idk. Just use WSL or switch to a proper OS.

### Building the Project

This project uses CMake for building. Run the following commands:

```
cd build
cmake [-DCMAKE_BUILD_TYPE=Release] ..
make
```

## Usage

### Running the Program

```
cd bin
./mandelbrot [options]
```

### Command-Line Arguments

| **Option** | **Description** |
| -----| ----- |
| `-h` / `--help`| Show help message and quit |
| `-e PATH` / `--env_path PATH`| Set path of environment (for saving and loading) |
| `-l FILE` / `--load FILE` | Set name of file to load settings from (relative to `-e`, has no effect when `-e` is not set)
| `-s FILE` / `--save FILE` | Set name of file to save settings to (relative to `-e`, has no effect when `-e` is not set)
| `--width WIDTH`| Set width of window in pixels (default: 800) |
| `--height HEIGHT` | Set height of window in pixels (default: 600) |
| `--max_re MAX` | Set maximum value of real part for initial view (default: 1.0) |
| `--min_re MIN` | Set minimum value of real part for initial view (default: -2.0) |
| `--cntr_re CNTR` | Set imaginary part of centre point for initial view (default: 0.0) |
| `--max_itrs ITER` | Set maximum number of iterations to make (default: 1000) |
| `--num_chnks_re NUM` | Set number of chunks in real direction (default: 20) |
| `--num_chnks_im NUM` | Set number of chunks in imaginary direction (default: 20) |
| `--zoom_fac FAC` | Set factor for one zoom stage (default: 0.5) |
| `--fps FPS` | Sets frames per second for intermediary updates (default: 30) |
| `--palette_idx IDX` | Set start index for colour palette (default: 4) |
| `--trip_mode MODE` | Sets "trip mode" type (default: 0) |

Command-line arguments take precedence over the JSON configuration.

### JSON Configuration Example

The JSON field names are identical to their corresponding command-line options:

```
{
  "max_itrs": 500,
  "num_chnks_re": 20,
  "num_chnks_im": 20,
  "zoom_fac": 0.5,
  "width": 800,
  "height": 600,
  "max_re": 1,
  "min_re": -2,
  "cntr_im": 0,
  "fps": 30,
  "palette_idx": 4,
  "trip_mode": 0
}
```

### Controls

| Key | Action |
| -----| ----- |
| `WASD` / &#8593;&#8592;&#8595;&#8594; | Move in the complex plane |
| `+` / `-` | Zoom in / out |
| `F5` | Save current view |
| `F9` | Load saved view |
| `ESC` / `q` | Exit program |
| `c` | Cycle colour palette |
| `t` | Enable "trip mode" (hold key) |

## License

This project is licensed under the MIT License.
