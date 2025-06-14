
# mandelbrot

A C99 program that visualizes the Mandelbrot set and allows interactive exploration using keyboard controls. Users can navigate through the complex plane, zoom in and out as well as save/load views. The program supports both command-line configuration and JSON settings files for flexibility.

## Features

-   **Real-Time Navigation**: Move through the Mandelbrot set using key presses.
    
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

-   [cutil](https://github.com/Acsyon/cutil) - My own C utility library.

## Installation

There are two modes of installation:

-   **Development**: This is the default. It lets CMake search for installed packages and is only available on Linux.

-   **Distributed**: This one is used to create a single, statically linked executable that can be distributed to other people without the need for external dependencies. The required libraries are included as git submodules and built from scratch. On Windows, it uses [MPIR](https://github.com/BrianGladman/mpir) as a drop-in replacement for GMP.

### Prerequisites

This project uses CMake as its build system, so make sure to have it installed.

Additionally, one or more git submodules need to be included depending on the installation mode. Since cutil is my own custom C library, there is no package for it. Therefore, it has to be included as submodule for both modes:

```sh
git submodule update --init submodules/cutil
```

For the "distributed" mode, it is easiest to load all submodules via:

```sh
git submodule update --init
```

### Preparing the "Development" Mode

Ensure you have the required libraries and build systems installed on your system (OpenMP comes automatically with GCC but is required for Clang):

#### Arch-based systems:

```sh
sudo pacman -S sdl2-compat gmp cjson [clang openmp]
```

#### Debian-based systems:

```sh
sudo apt install libsdl2-dev libgmp-dev libcjson-dev [clang libomp-dev]
```

#### Preparing the build

Run the following commands to configure CMake and prepare it for building:

```sh
cd build
cmake [-DCMAKE_BUILD_TYPE=<TYPE>] ..
```

Here, `TYPE` specifies the build type. Possible options are

-   **Release**: Enables the highest (sensible) compiler optimizations to improve perfomance (recommended).

-   **Debug**: Disables compiler optimizations and includes debug symbols into the executable.

-   **RelWithDebInfo** (release with debug info): Enable many compiler optimizations, but also include debug symbols.

### Preparing the "Distributed" Mode

Ensure that you have initialized all submodules. Thereafter, you can activate the "distributed" mode by setting the appropriate variable in CMake:

```sh
cd build
cmake -DMANDELBROT_DISTRIBUTED=TRUE [-DCMAKE_BUILD_TYPE=<TYPE>] ..
```

### Building the Project

After having configured the installation mode and regardless of the platform, CMake can also be used to build the project:

```sh
cd build
cmake --build .
```

## Unit Tests

If you &ndash; for whatever reason &ndash; want to run the unit tests, you have to load the git submodule for the "Unity" unit-test framework:

```sh
git submodule update --init submodules/Unity
```

Afterwards, CMake has to be set up with the appropriate variable. The tests will be built automatically if no target is specified. They can be run using CTest. All of this can be achieved by running the following commands:

```sh
cd build
cmake -DENABLE_TESTS=TRUE [-DMANDELBROT_DISTRIBUTED=TRUE] [-DCMAKE_BUILD_TYPE=<TYPE>] ..
cmake --build .
ctest
```

## Usage

### Running the Program

You can run the program either via the command line:

```sh
cd bin
./mandelbrot [options]
```

Alternatively, the executable can also be started via your file browser.

### Command-Line Arguments

| **Option** | **Description** |
| -----| ----- |
| `-h` / `--help`| Show help message and quit |
| `-d` / `--defaults`| Show default settings as JSON and quit |
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
| `--view_file FILE` | Sets file to read view from (default: "view.json") |
| `--address ADDRESS` | Sets address of server (default: "127.0.0.1") |
| `--port PORT` | Sets port of server (default: "10101") |

Command-line arguments take precedence over the JSON configuration.

### JSON Configuration Example

The JSON field names are identical to their corresponding command-line options:

```json
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
  "trip_mode": 0,
  "view_file": "view.json",
  "address": "127.0.0.1",
  "port": 10101
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

## Contact
For any questions or discussions, feel free to reach to me.
