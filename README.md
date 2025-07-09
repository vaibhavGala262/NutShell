🐚 NutShell — A Mini Custom Linux Shell

A lightweight, C-based shell implementation with features inspired by bash and zsh — built from scratch for learning and fun!


🚀 Features

📜 Command parsing (handles spaces, multiple args)
➕ Built-in commands
cdir → change directory (cd)
clearscreen → clears the terminal
pcd → shows current directory (pwd)
exit → exits the shell


🔄 Fork + exec for external commands
🧵 Multiple commands using ;
⬆️⬇️ Command history using GNU Readline
⚙️ Arrow key navigation, backspace, and editing support
⚡ Background jobs using &
📄 Modular code (parsers.c, executor.c, builtins.c, etc.)
🛠️ Makefile-based build system
🧠 Beginner-friendly codebase with comments


🧰 Build & Run
# Clone the repo
git clone https://github.com/vaibhavGala262/NutShell.git
cd NutShell

# Build it
make run


📋 Requirements
# Ensure these are installed
gcc (GNU Compiler)
make
libreadline-dev (for history & arrow support)

# For Linux (Ubuntu/Debian):
sudo apt update
sudo apt install -y build-essential libreadline-dev

