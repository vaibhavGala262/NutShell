# 🥜 NutShell - A Feature-Rich Unix Shell

<div align="center">

![NutShell Logo](https://img.shields.io/badge/NutShell-🥜-orange?style=for-the-badge)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20WSL-lightgrey?style=for-the-badge)](https://github.com/)

*A modern, feature-rich shell implementation with advanced capabilities, built in C for performance and portability.*

[Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Contributing](#-contributing)

</div>

---

## 🌟 Overview

NutShell is a custom Unix-like shell that combines the power of traditional shells like Bash with modern features and a clean codebase. It's designed for both interactive use and scripting, with a focus on robustness, debuggability, and ease of extension.

**Why NutShell?**
- 🔧 **Highly Configurable** - Runtime debug levels and customizable prompts
- 🧩 **Modular Design** - Clean separation of parsing, execution, and I/O
- 🛡️ **Memory Safe** - Rigorous memory management and leak prevention
- 📚 **Educational** - Perfect for learning systems programming concepts

Whether you're a developer looking for a customizable shell or a student studying systems programming, NutShell provides a complete platform with production-ready features.

---

## ✨ Features

### 🖥️ Core Shell Functionality
- **Command Execution** - Run any system command or built-in with full argument support
- **Pipelines** - Chain multiple commands (`ls | grep ".txt" | wc -l`)
- **Background Jobs** - Run processes in the background with `&` and job notifications
- **I/O Redirection** - Full support for `>`, `>>`, `<`, `2>`, and `&>` operators
- **Command History** - Navigate with arrow keys, persistent across sessions
  

### ⚡ Advanced Capabilities
- **Logical Operators** - Conditional execution with `&&` (AND) and `||` (OR)
- **Variable System** - Set, expand, and export variables (`name="John" && echo "Hello @name"`)
- **Built-in Commands** - `cdir`, `pcd`, `env`, `set`, `export`, `unset`, `history`, `debug`, `exit`
- **Signal Handling** - Graceful exit on Ctrl+C, Ctrl+D with proper cleanup
- **Customizable Prompts** - Colored, informative prompts showing current directory

### 🐛 Debug & Development Tools
- **Multi-Level Debugging** - From errors-only to verbose tracing
- **Runtime Debug Control** - Change debug levels with `debug <level>`
- **Build Configurations** - Multiple build modes via Makefile
- **Extensible Codebase** - Modular structure for easy feature addition

> **Example:** `make && ./program | grep error > log.txt || echo "Failed"`

---

## 📦 Installation

### Prerequisites
| Tool | Description | Installation |
|------|-------------|--------------|
| **GCC** | C Compiler | `sudo apt install gcc` (Ubuntu) |
| **Make** | Build Tool | `sudo apt install make` |
| **GNU Readline** | Command Line Editing | `sudo apt install libreadline-dev` |
| **Git** | Version Control | `sudo apt install git` |

### Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/vaibhavGala262/Nutshell.git
cd nutshell

# 2. Install dependencies (Ubuntu/Debian)
sudo apt update && sudo apt install gcc make libreadline-dev

# 3. Build NutShell
make release

# 4. Run NutShell
./bin/nutshell

# 5. (Optional) Install system-wide
sudo make install

# 6. (Optional) Uninstall
sudo make uninstall
```

### Install Paths

By default, `make install` places the binary in `/usr/local/bin/nutshell`.

You can override paths using standard variables:

```bash
# Install to a custom prefix (e.g., /opt/nutshell)
sudo make install PREFIX=/opt/nutshell

# Package-friendly staging (for .deb/.rpm builds)
make install DESTDIR=/tmp/pkg-root
```

### Platform-Specific Instructions

<details>
<summary>🐧 <strong>Linux (Ubuntu/Debian)</strong></summary>

```bash
sudo apt update
sudo apt install gcc make libreadline-dev git
```
</details>

---

## 📤 Binary Releases (Linux)

This repo includes a GitHub Actions workflow that builds a Linux binary on tag push.

### Create a Release

```bash
# Tag a version and push
git tag v1.0.0
git push origin v1.0.0
```

That will create a GitHub Release and attach a Linux binary named like:

```
nutshell-linux-x86_64
```

### Download Without Opening Releases

You can fetch the latest binary directly with the GitHub CLI:

```bash
gh release download --repo vaibhavGala262/Nutshell -p "nutshell-linux-*"
chmod +x nutshell-linux-*
./nutshell-linux-*
```

Or use a direct URL for a specific version:

```bash
curl -L -o nutshell-linux-x86_64 \
	https://github.com/vaibhavGala262/Nutshell/releases/download/v1.0.0/nutshell-linux-x86_64
chmod +x nutshell-linux-x86_64
./nutshell-linux-x86_64
```

<details>
<summary>🍎 <strong>macOS (Homebrew)</strong></summary>

```bash
brew install gcc make readline git
```
</details>

<details>
<summary>🪟 <strong>Windows (WSL)</strong></summary>

1. Install WSL with Ubuntu from Microsoft Store
2. In WSL terminal:
```bash
sudo apt update
sudo apt install gcc make libreadline-dev git
```
</details>

---

## 🚀 Usage
```bash
nutshell> ls -la           # List files with details
nutshell> pwd/pcd             # Print working directory  
nutshell> cdir /home/user    # Change directory
nutshell> exit             # Quit the shell
```

### Pipelines & Redirection
```bash
nutshell> ls | grep ".c" | wc -l > count.txt    # Count C files
nutshell> cat < input.txt >> output.txt         # Append file contents
nutshell> command 2> error.log                  # Redirect errors
nutshell> command &> all.log                    # Redirect everything
```

### Logical Operators
```bash
nutshell> make && ./program                     # Run if make succeeds
nutshell> ./script.sh || echo "Script failed"  # Run if script fails
nutshell> test -f file.txt && echo "Exists" || echo "Missing"
```

### Variables
```bash
nutshell> name="NutShell"              # Set variable
nutshell> echo "Welcome to $name"      # Use variable
nutshell> export PATH="$PATH:/new/path" # Export variable
nutshell> unset name                   # Remove variable
nutshell> env                          # Show all variables
```

### History & Debug
```bash
# History navigation
# Use ↑/↓ arrows or type 'history'

# Debug modes
nutshell> debug 4    # Enable verbose debug
nutshell> debug 0    # Disable debug
nutshell> debug      # Show current level
```

---

## 🔧 Build Options

| Command | Description | Use Case |
|---------|-------------|----------|
| `make release` | Optimized production build | **Default for users** |
| `make debug` | Full debug info (Level 3) | Development & debugging |
| `make verbose` | Maximum debug (Level 4) | Troubleshooting |
| `make dev` | Quick development build | Rapid iteration |
| `make profile` | Performance profiling | Optimization |
| `make clean` | Clean all build files | Fresh start |
| `make help` | Show all available targets | Reference |

### Runtime Debug Options
```bash
./bin/nutshell -v              # Verbose mode
./bin/nutshell --debug 2       # Warnings and errors
NUTSHELL_DEBUG=3 ./bin/nutshell # Environment variable
```

---

## 📁 Project Structure

```
nutshell/
├── 📁 src/                    # Source files
│   ├── 📄 main.c             # Main loop and entry point
│   ├── 📄 parsers.c          # Command parsing and tokenization
│   ├── 📄 executor.c         # Command execution logic
│   ├── 📄 builtins.c         # Built-in command implementations
│   ├── 📄 variables.c        # Variable management system
│   ├── 📄 history.c          # Command history functionality
│   ├── 📄 completion.c       # Tab completion system
│   ├── 📄 debug.c            # Multi-level debugging system
│   └── 📄 utils.c            # Utility functions
├── 📁 include/               # Header files
├── 📁 bin/                   # Compiled binaries
├── 📁 obj/                   # Object files (build artifacts)
├── 📄 Makefile              # Build system configuration
└── 📄 README.md             # This file
```

---

## 🛠️ Troubleshooting

<details>
<summary><strong>Command not found</strong></summary>

- Check if the command is in your `PATH`: `echo $PATH`
- Use the full path to the command: `/usr/bin/ls`
- Verify the command exists: `which command_name`
</details>

<details>
<summary><strong>Permission denied</strong></summary>

- Make the file executable: `chmod +x filename`
- Check file ownership: `ls -la filename`
- Run with appropriate permissions: `sudo command`
</details>

<details>
<summary><strong>Build errors</strong></summary>

- Verify all dependencies are installed
- Try a clean build: `make clean && make debug`
- Check compiler version: `gcc --version`
- Review error output carefully
</details>

> 💡 **Pro Tip:** Run with `-v` flag for verbose output when reporting issues!

---

## 🤝 Contributing

We welcome contributions! Here's how to get started:

1. **Fork** the repository
2. **Create** your feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Contribution Guidelines
- ✅ Include clear description of changes
- ✅ Add relevant tests for new features
- ✅ Update documentation as needed
- ✅ Follow existing code style
- ✅ Test on multiple platforms if possible

### Development Setup
```bash
git clone https://github.com/vaibhavGala262/Nutshell.git
cd nutshell
make run-debug   # Build with debug symbols
make run-verbose # For detailed debugging 
```

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- 📚 **GNU Readline** - For excellent command line editing capabilities
- 📜 **POSIX Standards** - For shell behavior specifications  
- 🌍 **Open Source Community** - For inspiration, tools, and continuous learning
- 💡 **Systems Programming Community** - For sharing knowledge and best practices

---

<div align="center">

### 🌟 Star this project if you found it helpful!

**Built with passion for systems programming**  
*Crack open a new shell experience! 🥜*

[![GitHub stars](https://img.shields.io/github/stars/vaibhavGala262/Nutshell.svg?style=social&label=Star)](https://github.com/vaibhavGala262/Nutshell)
[![GitHub forks](https://img.shields.io/github/forks/vaibhavGala262/Nutshell.svg?style=social&label=Fork)](https://github.com/vaibhavGala262/Nutshell/fork)

[⬆ Back to Top](#-nutshell---a-feature-rich-unix-shell)

</div>