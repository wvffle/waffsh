waffsh
---
This shell is a university project. You ***should not*** use it.

## The requirements
- [x] Ability to start shell in an interactive mode
- [x] Ability to start basic shell scripts with `#!/usr/bin/waffsh`
- [x] Running process in background (`sleep 10 &`)
- [x] Working pipes (`ls -1 | sort`)
- [x] Working `>>` redirect (`ls >> ls.txt`)
- [x] Command history stored in `~/.waffsh_history`
- [x] View command history when SIGQUIT received

## Personal goals
- [x] Support both `>` and `>>` redirects respectively
- [ ] Escape special characters (`>`, `&`, `#`)
- [ ] Add `cd`, `exit` builtins
- [ ] Support redirects in background when `&` is a second token (`ls > ls.txt &`)
- [ ] Support quotes (`grep -E '"'`, `grep -E '"'`, `grep -E '\''`)
- [ ] VIM-like interface
- [ ] Handle resizing

## Design limitations
The shell allows only basic command execution. No if-statements, no loops. It simply parses the command and executes it.

### Redirects
Redirects accept only one token after them, which is expected to be a file.
The only exception is when the next token is `&` which means that it'll run in background.

### Running in background
When parser meets `&` character, it skips everything past it. That said things like that won't work:
```sh
echo Hello & hi # prints 'Hello' in background
```

## VIM-like interface
VIM-like interface is opened only when shell is interactive.

### Insert mode
In insert mode user is presented with an interactive shell. 
At the bottom is fixed command input where user can type commands.
```text
┌┨INSERT┠──────────────────┐
│ hello                    │
│                          │
│                          │
├──────────────────────────┤
│ ~/ > ls -1 | sort▊       │
└──────────────────────────┘
```

### Normal mode
In normal mode user can scroll through the output of previous commands.
```text
┌┨NORMAL┠───────────┨10/34┠┐
│░hello░░░░░░░░░░░░░░░░░░░░│
│ hello                    │
│ hello                    │
├──────────────────────────┤
│ ~/ > ls -1 | sort        │
└──────────────────────────┘
```

### History mode
In history mode user can scroll through the command history and select one that he can then edit.
```text
┌┨HISTORY┠────────────┨2/2┠┐
│                          │
│ ls -1                    │
│░ls░-1░>░ls.txt░░░░░░░░░░░│
├──────────────────────────┤
│ ~/ > ls -1 > ls.txt      │
└──────────────────────────┘
```
