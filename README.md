# Wavefront98
Wavefront OBJ and MTL parser C++ 98

### Compatibility and Dependencies
- C++ 98 Standard
- Standard Template Library (STL)

This project is designed to be compatible with C++98 and utilizes solely C++ Standard Template Library (STL) components. It has been thoughtfully engineered to function independently without the need for any external libraries or dependencies. This design ensures the project remains self-contained, promoting simplicity and ease of integration into your development environment.<br>

*If you require the use of **C++11** or newer, see [WavefrontOBJ](https://github.com/StefanJohnsen/WavefrontOBJ) and [WavefrontMTL](https://github.com/StefanJohnsen/WavefrontMTL) repository. 

### OS Support
- Windows
- Linux
- macOS

### Speed and memory
- By supporting C++ 98 Standard only and without use of multithreading, we have achieved a good level of speed.
- This solution boasts minimal memory usage, typically averaging twice the file size in memory consumption.
  
### Usage
Copy `WavefrontOBJ.h`, `WavefrontMTL.h` and `WavefrontMTL.cpp` to your project and include the file.

```
#include "WavefrontOBJ.h"
#include "WavefrontMTL.h"
```

### Parsed examples
*Wavefront98 have been integrated into the [Polyscope 3D viewer](https://github.com/nmwsharp/polyscope), showcasing the parsing capabilities.*

![Polyscope](https://github.com/StefanJohnsen/WavefrontOBJ/blob/main/Pictures/polyscope.jpg)
*[3D model Mars Perseverance Rover](https://mars.nasa.gov/resources/25042/mars-perseverance-rover-3d-model/),  from NASA's Jet Propulsion Laboratory*

### Description
This repository is a faithful replica of [WavefrontOBJ](https://github.com/StefanJohnsen/WavefrontOBJ) and [WavefrontMTL](https://github.com/StefanJohnsen/WavefrontMTL), meticulously maintained for your convenience. The comprehensive documentation and resources from those repositories remain unchanged. The only distinction lies in the programming language used – this codebase is crafted with C++98, while the original repositories are based on C++11.

Please refer to the corresponding repositories linked above for all the information you require. Whether you're interested in the capabilities of WavefrontOBJ or the utilities of WavefrontMTL, you'll find everything you need to know.

## Benchmark C++98
The benchmark was conducted on a computer with the following specifications:

- **Processor:** Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz   2.59 GHz
- **Memory:** 16.0 GB
- **Operating System:** 64-bit operating system, x64-based processor
- **Compiler:** Visual Studio Version 2022 (v143, C++ 14 Standard)
- **Release Mode:** Visual Studio release mode was used for benchmarking.

| File Name    | File Size (KB) | Load time (Milliseconds)  | Memory Usage   |
|--------------|---------------:|:-------------------------:|:--------------:|
|  rungholt    |     269 215    |           1370            |    594MB       |
|  powerplant  |     798 723    |           3788            |    1.6GB       |
|  san-miguel  |   1 116 252    |           5152            |    2.1GB       |

Time was evaluated through 10 iterative runs, calculating the average execution time.<br>
Memory usage was analyzed using Visual Studio 2022 C++17 Diagnostic Tools in Release mode.<br>
*See section [Benchmark code in WavefrontOBJ](https://github.com/StefanJohnsen/WavefrontOBJ#benchmark-code)*<br>

The files above are sourced from [**Morgan McGuire, Computer Graphics Archive, July 2017**](https://casual-effects.com/g3d/data10).<br><br>
*The benchmark results vary based on the computer's hardware and software configuration.*

## References
The following sources have been utilized in developing Wavefront98.

[Paul Bourke: Object Files (OBJ)](http://paulbourke.net/dataformats/obj/)

[Paul Bourke: MTL material format (Lightwave, OBJ)](http://paulbourke.net/dataformats/mtl/)

[Wikipedia: Wavefront .obj file](https://en.wikipedia.org/wiki/Wavefront_.obj_file)

[FileFormat.info: WaveFront Material (.mtl) File Format](https://www.fileformat.info/format/material/)

## License
This software is released under the GNU General Public License v3.0 terms.<br> 
Details and terms of this license can be found at: https://www.gnu.org/licenses/gpl-3.0.html<br><br>
For those who require the freedom to operate without the constraints of the GPL,<br>
a commercial license can be obtained by contacting the author at stefan.johnsen@outlook.com
