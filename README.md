# cppdll

A C++ library designed to modify and run Windows shortcuts programmatically.

## Architecture Overview

The `cppdll` repository is structured around a core functionality to manipulate Windows shortcuts using the COM interface provided by the ShellLink object. The architecture consists of several key components:

- **Shortcutma.cpp**: Contains the exported function `ModifyShortcutAndRun`, which modifies an existing shortcut or creates a new one if it doesn't exist, and then runs the target application.
  
- **Shortcutmanager.cpp**: Similar to `Shortcutma.cpp`, this file also contains the `ModifyShortcutAndRun` function but uses the Unicode version of the ShellLink interface (`IShellLinkW`).

- **main.cpp**: Demonstrates how to use the functions from `Shortcutma.cpp` and `Shortcutmanager.cpp`. It initializes COM, creates a ShellLink object, sets properties like the target path, and saves the shortcut.

## Tech Stack

- **Programming Language**: C++
- **Libraries/Frameworks**:
  - Windows API (`windows.h`)
  - Shell Object Interfaces (`shobjidl.h`, `shlguid.h`)
  - COM (Component Object Model) for creating and managing shell links
- **Build System**: None specified; assumes a standard C++ build environment

## Getting Started / Setup Guide

### Prerequisites

1. **Windows Environment**: This library is designed to run on Windows operating systems.
2. **C++ Compiler**: Ensure you have a C++ compiler installed, such as MSVC (Microsoft Visual Studio).

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/cppdll.git
   cd cppdll
   ```

2. **Build the Project**:
   - Open the project in your preferred IDE or use a command-line build tool like `cl.exe` (part of MSVC).
   - Example using `cl.exe`:
     ```bash
     cl /EHsc main.cpp Shortcutma.cpp Shortcutmanager.cpp ole32.lib shell32.lib
     ```

### Usage

1. **Modify and Run a Shortcut**:
   - Compile the project to generate an executable.
   - Run the executable, passing the new target path as a command-line argument.

2. **Integration with rundll32**:
   - The exported functions `ModifyShortcutAndRun` can be called directly from `rundll32.exe`.
   - Example command:
     ```bash
     rundll32.exe yourdll.dll,ModifyShortcutAndRun "C:\Path\To\NewTarget.exe"
     ```

### Notes

- Ensure that the paths specified in the code (e.g., shortcut locations) are valid and accessible.
- Proper error handling is implemented to manage COM initialization failures and file operations.

This setup provides a robust foundation for modifying and running Windows shortcuts programmatically using C++.