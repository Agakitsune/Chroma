# Chroma

Chroma is a raster graphics editor developed in C++ inspired by Gimp and influenced by Aseprite software.


## Overview

Chroma aim to provide an easy-to-use and ready-to-go with most useful features for casual graphical editing.

- Light image editing
- Easy to use
- Ready to go

## Quick start

### Prerequisities

[Download Cmake here](https://cmake.org/)

[Cmake dependencies](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md)

[GCC](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md)

### Installation

At the root of the project:

```bash
cmake -B build
```

Once done, go to /build folder and enter:

```bash
cd build
cmake --build . && ./chroma
```

## Basic Controls

`Left Click`: Draw with current color.

`Right Click`: Over the colorpicker, register new color in palette

`Scroll`: Zoom in/out.

## Features

### Common Commands

| Functionnalities       | Description                                    |
| -----------------------|------------------------------------------------|
| `new canva`            | Create blank canva                             |
| `open file`            | Open any file from your system                 |
| `save file`            | Save edits                                     |
| `exit`                 | Close software                                 |
| `undo`                 | Reverse last action                            |
| `redo`                 | Put back deleted content                       |
| `flip horizontal`      | Flip horizontally current picture              |
| `flip verical`         | Flip vertically current picture                |
| `colorpicker`          | Colorpicker tool                               |
| `colorpicker gradient` | Colorpicker tool to pinpoint colors            |
| `save in palette`      | Register color for later usage                 |

### Visual examples

`New canva`:

![Chroma Demo](screenshots/new_file.gif)

`Flip image`:

![Chroma Demo](screenshots/flips.gif)

`Colorpicker`:

![Chroma Demo](screenshots/colorpicker.gif)

## Project Structure

```
Chroma/
├── build/       
├── include/          
├── screenshots/             
└── src/         
```

## Technical Stack

| Component      | Technologies                                                                 |
| -------------- | -----------------------------------------------------------------------------|
| Language       | C++20                                                                        |
| Graphics API   | SDL3 Renderer Vulkan                                                         |
| User Interface | Dear ImGui                                                                   |
