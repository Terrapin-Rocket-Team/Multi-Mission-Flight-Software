---
title: Installation
---

You may skip over this installation step if you are working in a repo that already uses MMFS and you are just looking to get introduced.

!!! warning "Important!"
    In order for PIO to recognize that you are working on a PIO project, you *must* open VSCode in the root directory of that project. that is, the directory that has the `platformio.ini` file in it. Without this, PlatformIO **will not initialize** and you will be unable to build or use proper Intellisense.
---

## Prerequisites

Before you begin, make sure you have the following:

- Basic knowledge of C++ programming. (knowledge of what a pointer is and how to use one)
- VSCode and the PlatformIO (PIO) extension installed. [[Installation Guide](https://docs.platformio.org/en/latest/core/installation/index.html)]
- Basic knowledge of VSCode and the PlatformIO interface [[Interface cheat-sheet](#)]
- Basic knowledge of Git and GitHub. [[TRT Git/Github Guide](#)]
- Access to a Teensy 4.1. 

---

## Installation


### Create a new PlatformIO project

Pick any folder to create it in, and create a new project. We strongly recommend choosing the arduino framework and the Teensy 4.1 board, as MMFS has not been tested on *any* other system.

### Modify the `platformio.ini` file

Add a dependency to MMFS by adding the highlighted lines to your `platformio.ini` file, found in your project's root directory.
```ini linenums="10" hl_lines="5-6" title=""
[env:teensy41]
platform = teensy
board = teensy41
framework = arduino
lib_deps =
    https://github.com/Terrapin-Rocket-Team/Multi-Mission-Flight-Software.git#v3.0.0
```

!!! info 
    You may add multiple dependencies by appending new indented lines under `lib_deps`.
!!! tip 
    We recommend always attaching the version specifier to the end of the url, as MMFS undergoes frequent breaking changes. For github links, this looks like `#{tag}`. We give our releases (a.k.a. tags) semantic versioning[^sv] numbers like `v3.0.0`

Now save the file. You should notice PIO start to download MMFS and all of its dependencies. It may take a few minutes.

### Add `MMFS.h` to `main.cpp`

Looking at the folder structure, PIO should have created a `src` folder with a `main.cpp` in it. It has some basic example functions that you may ignore or delete. To link MMFS to this main file though, we need to `#include` it:

```cpp hl_lines="2" title=""
#include <Arduino.h>
#include <MMFS.h>

// put function declarations here:
int myFunction(int, int);
```

### Build the project

The last thing to do to make sure that everything went well is to simply build the project.

You can use any of PIO's `build` buttons to achieve this. If you don't know where the buttons are, we recommend using the toolbar on the bottom of the screen, where the `build` command is represented by the checkmark.

The terminal should spit out `========[SUCCESS] Took ##.## seconds=======`. If not, first try and see if you can understand the error. If you get stuck, please feel free to message any of the club members with experience working in PlatformIO or MMFS.

## Conclusion

That's all there is to installing the library. You're now ready to move on to using it! Hopefully that part will be just as straightforward.

[^sv]: We always use this format, but we don't always follow correct [semantic versioning procedures](https://semver.org/).