

# **Meshless Deformations Based on Shape Matching**

Implementation of Müller, Matthias, et al. "Meshless deformations based on shape matching." ACM transactions on graphics (TOG) 24.3 (2005): 471-478.


## 1. Compile and run this project:

### Step 1: Clone the source code from GitLab and enter the codebase folder

    git clone https://github.com/zhiyuXiao1221/Meshless-Deformations-Based-on-Shape-Matching.git
    cd physical_computing_final_project

### Step 2: Build the project using CMake:

**Substep 2.0: Install CMake**

- **Windows**: Visit https://cmake.org/download/ and download the latest version.  
- **Linux**: `sudo apt-get install cmake`
- **OSX**: `brew install cmake` (using [Homebrew](https://brew.sh/))

**Substep 2.1.0: Install IDE (Windows only)**
  - If you are using Windows and you don't have Visual Studio, please install [Visual Studio 2017](https://visualstudio.microsoft.com/vs/older-downloads/) , as well as it's [C/C++ support](https://docs.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-150) 
  - If you have already installed another version of VS (not Visual Studio 15 2017) or if you are not sure about the version, [check the version](https://stackoverflow.com/questions/33917454/cmake-how-to-specify-the-version-of-visual-c-to-work-with) by running 
  ` camke --help `
  in your command.   
  **Find setup.bat in your script folder, and change "Visual Studio 17 2015" to your own version**, or you might get an error at step 2.2

**Substep 2.1.1: Install Dependencies (Linux/OSX only)**
- **Linux**: `sudo apt-get install freeglut3-dev libglew-dev`
- **OSX**: `brew install glew freeglut`

  
**Substep 2.2: Run setup.sh/bat**

    .\scripts\setup.bat [Windows]
    ./scripts/setup.sh [Linux/OSX]

Note: you need to run the command in your root folder (physical_computing_final_project). Otherwise your computer will not be able find the cmake file.  
 

### Step 3: Compile and run the code

To test if everything works correctly you can run:

    .\scripts\run_assignment.bat final_project [Windows]
    ./scripts/run_assignment.sh final_project [Linux/Mac]

This will:

- Compile the code for the assignment 1 and any dependencies
- Popup a window and show an OpenGL window
