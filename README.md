Gym Management System

A robust, console-based Gym Management System written in C++. This application provides a complete solution for managing trainees, trainers, and class schedules for a fitness center.

This guide focuses on setting up and running the project on Windows using Visual Studio Code.

Requirements

Visual Studio Code

Git (for cloning the repository)

C/C++ Extension for VS Code (from Microsoft)

A C++ Compiler: The MinGW-w64 toolchain is recommended.

Setup and Run Instructions
Step 1: Clone the Repository

First, get the project code from GitHub. Open a terminal (like Command Prompt or Git Bash) and run this command:

Generated bash
git clone <your-repository-url>
cd <your-repository-folder>


Replace <your-repository-url> with the actual URL of your GitHub repository.

Step 2: Install a C++ Compiler (if you don't have one)

VS Code needs a compiler to build the code. If you don't have one, follow these steps to install MinGW.

Install MSYS2: Go to the MSYS2 website and run the installer.

Install MinGW Toolchain: After installation, an MSYS2 terminal will open. Run the following command:

Generated bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
Bash
IGNORE_WHEN_COPYING_END

Press Enter to select the default (all packages) and Y to confirm.

Add Compiler to Windows PATH:

The compiler's path is typically C:\msys64\ucrt64\bin. Copy this path.

Press the Windows Key, type Edit the system environment variables, and open it.

Click Environment Variables....

Under "System variables", select Path and click Edit....

Click New, paste the path (C:\msys64\ucrt64\bin), and click OK on all windows.

Verify: Open a new Command Prompt and type g++ --version. You should see the compiler version details.

Step 3: Set Up and Run in VS Code

Open Project Folder: In VS Code, go to File > Open Folder... and open the project folder you cloned in Step 1.

Install Extension: If you haven't already, go to the Extensions view (Ctrl+Shift+X), search for C/C++, and install the one from Microsoft.

Run the Code:

Open the main.cpp file.

Click the Play Button (▶) in the top-right corner of the editor.

If prompted, select C/C++: g++.exe build and debug active file.

The TERMINAL panel will open at the bottom, compile the code, and run the program. You can now use the Gym Management System.

How to Use the Application

After running the program, you will be prompted to select your role: admin, trainer, or trainee.

Admin Login:

The first time you run the application, a default admin account is created.

Username: admin

Password: admin123

Trainee/User Flow:

New users should select trainee and then 1. Register.

Existing users can select trainee and then 2. Login.

Trainer Flow:

Trainers must be added by an Admin first.

They can then log in using the ID and password set by the admin.

Data Files

The application will automatically create and use the following files in your project folder to save data:

admins.txt

trainers.txt

trainees.txt

classes.txt
