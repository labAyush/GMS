GYM MANAGEMENT SYSTEM

A robust, console-based Gym Management System written in C++. This application provides a complete solution for managing trainees, trainers, and class schedules for a fitness center.

This guide is tailored for running the project on Windows using Visual Studio Code.

🛠️ REQUIREMENTS

Before you begin, ensure you have the following installed:

Visual Studio Code

Git (for cloning the repository from GitHub)

C/C++ Extension for VS Code (the official extension from Microsoft)

A C++ Compiler: We highly recommend the MinGW-w64 toolchain.

⚙️ SETUP AND RUN INSTRUCTIONS

Follow these steps to get the project up and running on your machine.

STEP 1: CLONE THE REPOSITORY

First, get the project code from GitHub. Open a terminal (like Command Prompt or Git Bash) and run this command:

git clone https://github.com/labAyush/GMS

cd GMS


STEP 2: INSTALL A C++ COMPILER

If you don't already have the g++ compiler, follow these steps to install MinGW.

Install MSYS2: Go to the MSYS2 website and run the installer.

Install MinGW Toolchain: After installation, an MSYS2 terminal will open. Run the following command:


pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain


Press Enter to select the default (all packages) and Y to confirm.

Add Compiler to Windows PATH:

The compiler's path is typically C:\msys64\ucrt64\bin. Copy this path.

Press the Windows Key, type Edit the system environment variables, and open it.

Click Environment Variables....

Under "System variables", select the Path variable and click Edit....

Click New, paste the path (C:\msys64\ucrt64\bin), and click OK on all windows.

Verify the Installation: Open a new Command Prompt and type the following command. You should see the compiler version details.

g++ --version


STEP 3: RUN IN VISUAL STUDIO CODE

Open Project Folder: In VS Code, go to File > Open Folder... and open the project folder you cloned in Step 1.

Install C/C++ Extension: If you haven't already, go to the Extensions view (Ctrl+Shift+X), search for C/C++, and install the one from Microsoft.

Run the Code:

Open the main.cpp file in the editor.

Click the Play Button (▶) in the top-right corner of the window.

If prompted, select C/C++: g++.exe build and debug active file.

The TERMINAL panel will open at the bottom, compile the code, and run the program. You can now interact with the Gym Management System directly inside VS Code.

🚀 HOW TO USE THE APPLICATION

After running the program, you will be prompted to select your role: admin, trainer, or trainee.

Admin Login:

The first time you run the application, a default admin account is created.

Username: admin

Password: admin123

Trainee/User Flow:

New users should select trainee and then choose 1. Register.

Existing users can select trainee and then choose 2. Login.

Trainer Flow:

Trainers must be added by an Admin first.

Once added, they can log in using the ID and password set by the admin.

📂 DATA FILES

The application will automatically create and use the following text files in your project folder to save all data:

admins.txt

trainers.txt

trainees.txt

classes.txt
