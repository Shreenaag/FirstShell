# Shell Project - Functionality Documentation

## Project Overview
This is a custom **Unix-like shell implementation** written in C that replicates the functionality of bash/sh. It provides an interactive command-line interface (REPL - Read-Eval-Print-Loop) with built-in commands, process management, and colored terminal output.

---

## Project Architecture

### Core Components

#### 1. **shell.c / shell.h** - Main Shell Engine
**Purpose:** Core command-line interpreter and REPL loop

**Key Functions:**
- `main()` - Main event loop that continuously reads and executes commands
- `cell_read_line()` - Reads user input and displays prompt with current directory and exit status
- `cell_split_line()` - Tokenizes input string into command arguments
- `cell_exec()` - Determines if command is built-in or external, then executes it
- `cell_launch()` - Forks and executes external programs

**Prompt Display:**
- Shows current working directory (from `Getcwd()`)
- Displays exit status of last command in red if non-zero
- Uses colored emojis (🦠) for visual distinction
- Format: `🦠[/path/to/dir]🦠 >` or `🦠[/path/to/dir][exit_code]🦠 >`

---

#### 2. **builtin.c** - Built-in Commands
**Purpose:** Implements shell built-in commands (run in shell process, not forked)

**Built-in Commands:**

| Command | Arguments | Description |
|---------|-----------|-------------|
| `env` | None | Displays all environment variables (from `environ` array) |
| `echo` | `[-n] [text]` | Prints text to stdout; `-n` flag suppresses newline |
| `exit` | None | Exits shell with animated spinner loading sequence |

**Data Structure:**
```c
typedef struct s_builtin {
    const char *builtin_name;
    int (*foo)(char **av);  // Function pointer to implementation
} t_builtin;
```

**Execution Plan for Built-in Commands:**
1. Command is NOT forked (runs in parent shell process)
2. Directly calls corresponding function
3. Can modify shell state (unlike external commands)

---

#### 3. **utils.c** - Utility Functions and Wrappers
**Purpose:** Provides error-handling wrappers for system calls and utilities

**Memory Management:**
- `Malloc()` - Safe malloc wrapper with error checking
- `Realloc()` - Safe realloc wrapper with error checking

**System Call Wrappers:**
- `Fork()` - Creates child process; exits on failure
- `Execvp()` - Replaces process with new program; errors if command not found
- `Wait()` - Waits for child process and extracts exit status
- `Waitpid()` - Platform-specific process waiting
- `Getline()` - Reads line from input stream

**File System Operations:**
- `Getcwd()` - Gets current working directory
- `Chdir()` - Changes current directory (cd command)

**UI Elements:**
- `dbzSpinnerLoading()` - Animated spinner display on exit (13 frames, 421ms per frame)

**Error Handling:**
- All wrappers include terminal color codes for error messages
- Uses POSIX error handling macros (`WIFEXITED`, `WEXITSTATUS`)
- Colored output with RED/GREEN/CYAN/YELLOW/RST codes

---

#### 4. **tokens.c** - Tokenization Helper
**Purpose:** Demonstration/helper file for command parsing

**Function:**
- `cell_spilt_line()` - Splits command string into tokens using space delimiter

---

## Program Flow (REPL Loop)

```
┌─────────────────────────────────────────┐
│  START SHELL                            │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│ cell_read_line()                        │
│ - Display prompt with CWD and status    │
│ - Read user input                       │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│ cell_split_line()                       │
│ - Tokenize input by whitespace          │
│ - Return array of arguments             │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│ cell_exec()                             │
│ - Check if command is built-in          │
└──────────────┬──────────────────────────┘
               │
       ┌───────┴────────┐
       │                │
   Built-in?        External?
       │                │
       ▼                ▼
┌────────────┐   ┌──────────────────┐
│Run directly│   │cell_launch()     │
│Set status  │   │- Fork()          │
└────────────┘   │- Execvp()        │
       │         │- Wait()          │
       └────────►└──────────────────┘
               │
               ▼
         ┌─────────────┐
         │ Loop again? │─No─► EXIT
         │   (getline) │
         └──────yes────┘
               │
               ▼
         [Back to read_line]
```

---

## Command Execution Modes

### 1. **Built-in Commands** (No Fork)
- Executed in parent shell process
- Can modify shell state directly
- Examples: `env`, `echo`, `exit`
- Return status stored in global `status` variable

### 2. **External Commands** (Fork + Exec)
- Child process created via `Fork()`
- Child replaces with new program via `Execvp()`
- Parent waits for child via `Wait()`
- Exit status captured and stored

---

## Data Structures

### Global Variables
```c
int status = 0;              // Stores exit status of last command
t_builtin g_builtin[] = {...};  // Array of built-in commands
```

### Command Token Array
- Dynamic array allocated by `Malloc()`
- Grows as needed with `Realloc()` (doubles size when full)
- NULL-terminated for use with `execvp()`
- Freed after each command execution

---

## Terminal Features

### Color Codes
- **RED** (`\033[31m`) - Errors and exit status
- **GREEN** (`\033[32m`) - Success/positive status
- **YELLOW** (`\033[33m`) - Loading/warnings
- **CYAN** (`\033[36m`) - Directory display
- **RST** (`\033[0m`) - Reset to default

### Prompt Components
- Emoji decorators: 🦠 (bacteria emoji)
- Current directory in CYAN
- Non-zero exit code in RED
- Example: `🦠[CYAN/home/user[RED2RST]🦠 >`

### Exit Animation
- 13-frame spinner showing progress
- Each frame displays for ~421ms
- Final frame: `[===========💥]` followed by ✅ message

---

## Error Handling Strategy

**All system calls are wrapped** to provide:
1. **Automatic error checking** - Detects failures immediately
2. **Colored error output** - RED color for visibility
3. **Graceful termination** - Exits with appropriate status codes
4. **Informative messages** - Uses `perror()` for system error descriptions

**Used Exit Codes:**
- `EX_OK` (0) - Successful execution
- `EX_OSERR` - Operating system error (fork failure)
- `EX_UNAVAILABLE` - Command not found
- `EXIT_FAILURE` - Memory/general failure

---

## Memory Management

- **Dynamic allocation** for command arguments array
- **Reallocation** when buffer size exceeded
- **Cleanup** after each command with `free()`
- **Error handling** with safe wrapper functions
- **No memory leaks** - explicit freeing after command execution

---

## Supported System Calls

| Function | Purpose | Wrapper | Error Handling |
|----------|---------|---------|-----------------|
| `fork()` | Process creation | `Fork()` | Exits on failure |
| `execvp()` | Program execution | `Execvp()` | Exits on failure |
| `wait()` | Child process sync | `Wait()` | Extracts exit status |
| `waitpid()` | PID-specific waiting | `Waitpid()` | Platform-aware |
| `getcwd()` | Get working directory | `Getcwd()` | Prints error |
| `chdir()` | Change directory | `Chdir()` | Prints error |
| `getline()` | Read user input | `Getline()` | Handles EOF cleanly |
| `malloc()`/`realloc()` | Memory allocation | `Malloc()`/`Realloc()` | Exits on failure |

---

## Design Patterns

1. **REPL Pattern** - Continuous read-eval-print-loop
2. **Wrapper Functions** - System call safety and consistency
3. **Function Pointers** - Dynamic command dispatch for built-ins
4. **Error-First Design** - Aggressive error checking
5. **Modular Organization** - Separate files for different concerns

---

## Limitations & Future Enhancements

**Current Limitations:**
- No pipes or redirection
- No background process support (`&` operator)
- Limited built-in commands
- No command history
- No tab completion
- Relative paths only (limited directory support)

**Potential Enhancements:**
- Implement `cd` command for directory navigation
- Add `|` pipe support
- Implement `>`, `<` redirection
- Background process execution with `&`
- Command history tracking
- Signal handling (SIGINT, SIGCHLD)
- Path resolution and `cd` built-in
- More built-ins: `pwd`, `ls`, `cat`, etc.

---

## Compilation & Execution

**Files to compile:**
- `shell.c`, `shell.h` - Main shell
- `builtin.c` - Built-in commands
- `utils.c` - Utility functions
- `tokens.c` - Tokenization (if used)

**Typical compilation:**
```bash
gcc -o shell shell.c builtin.c utils.c tokens.c
./shell
```

**Platform Requirements:**
- POSIX-compliant system (Linux, macOS, BSD)
- C standard library with system calls
- Terminal with ANSI color support (optional, but recommended)

---

## Summary

This shell project demonstrates:
✅ **Process Management** - Fork and exec
✅ **Error Handling** - Comprehensive wrapper functions
✅ **Memory Safety** - Dynamic allocation with cleanup
✅ **User Interface** - Colored prompts and animations
✅ **Command Dispatch** - Built-in vs. external command routing
✅ **REPL Implementation** - Interactive command-line loop
✅ **System Integration** - Direct system call usage with POSIX standards

The project successfully implements a functional shell that can execute both built-in and external commands with proper process management and error handling.
