# Custom Shell Project

## Team Members
- Cody Robards

## Repository Contents
- `shell.c`: Source code for the shell implementation, including the command loop, built-in commands, and process management logic.
- `Makefile`: Helper targets for compiling (`make shell`), running (`make run`), and cleaning build artifacts (`make clean`).
- `README.md`: Project overview, usage instructions, and implementation notes (this document).

## Building and Running the Shell
1. Ensure the development system has `gcc` and standard POSIX build tools installed.
2. Compile the shell using the provided Makefile:
   ```sh
   make shell
   ```
3. Run the shell binary through the Makefile target:
   ```sh
   make run
   ```
   Alternatively, execute the compiled program directly with `./shell`.
4. Clean build artifacts when needed:
   ```sh
   make clean
   ```

To compile manually without the Makefile, run:
```sh
gcc -std=c11 -Wall -Wextra -pedantic -o shell shell.c
```

## Shell Usage and Features
- **Prompt Format:** Displays the current working directory in blue followed by a `$` symbol (e.g., `/workspace/os_project_1$`).
- **Built-in Commands:**
  - `cd [path]` changes directories and updates the `PWD` environment variable. Without an argument, it defaults to `$HOME` (or `.` if unset).
  - `exit` ends the shell session.
  - `showpid` prints the recent child process IDs recorded by the shell (up to the last five).
- **External Commands:** Non-built-in commands are executed via `fork`/`execvp` with standard argument tokenization using whitespace delimiters. Up to 16 tokens of 64 characters each are supported per command line.
- **PID History:** The shell stores the last five child process IDs in a circular buffer. This allows quick inspection of recently executed processes through the `showpid` built-in.

## Challenges and External Resources
- **Token Limits and Error Handling:** Enforcing maximum token counts and lengths required careful validation to avoid buffer overruns and provide informative error messages.
- **Directory Management:** Ensuring `cd` updated the working directory and `PWD` environment variable demanded extra error handling around `chdir`, `getcwd`, and `setenv`.
- **Resource Consultation:** Referenced POSIX manual pages for `fork(2)`, `execvp(3)`, `waitpid(2)`, `strtok_r(3)`, `chdir(2)`, and `setenv(3)` to confirm correct usage and error handling semantics.

## Development Notes for Evaluators
- The shell loops until `exit` or EOF (`Ctrl+D`) is received, gracefully terminating after printing a newline.
- Input longer than 1024 characters is truncated by `fgets`; overly long tokens or more than 16 tokens trigger explicit error messages and skip command execution.
- Failed `execvp` calls print the system error before the child exits with failure status.
- Built-in `cd` does not spawn a child process; all other commands fork and wait synchronously, recording each completed child PID in the history buffer.

## Known Limitations
- Background execution (`&`), pipelines, and I/O redirection are not supported.
- The PID history holds only the five most recent child processes.

