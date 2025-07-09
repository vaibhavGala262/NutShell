🐚 NutShell — A Mini Custom Linux Shell
A lightweight, C-based shell implementation inspired by bash and zsh, built from scratch for learning and fun! 🚀

✨ Features

📜 Command Parsing: Supports spaces and multiple arguments for seamless command handling.
🛠️ Built-in Commands:
cdir: Change directory (cd equivalent).
clearscreen: Clears the terminal screen.
pcd: Displays the current working directory (pwd equivalent).
exit: Exits the shell gracefully.


🔄 Fork + Exec: Executes external commands efficiently.
🧵 Multiple Commands: Run multiple commands in a single line using ;.
⬆️⬇️ Command History: Powered by GNU Readline for easy command recall.
⚙️ Arrow Key Navigation: Full support for arrow keys, backspace, and command-line editing.
⚡ Background Jobs: Run commands in the background with &.
📄 Modular Codebase: Organized into parsers.c, executor.c, builtins.c, and more.
🛠️ Makefile-Based Build: Simple and efficient build system.
🧠 Beginner-Friendly: Well-commented code for easy understanding.


🧰 Build & Run
# Clone the repository
git clone https://github.com/vaibhavGala262/NutShell.git
cd NutShell

# Build and run the shell
make run


📋 Requirements
To build and run NutShell, ensure the following are installed on your system:

GCC: GNU Compiler Collection for compiling the C code.
Make: For managing the build process via the Makefile.
libreadline-dev: For command history and arrow key support.

🐧 For Linux (Ubuntu/Debian):
# Update package lists and install dependencies
sudo apt update
sudo apt install -y build-essential libreadline-dev

