waffsh
---
This shell is a university project. You ***should not*** use it.

It is available on GitHub: https://github.com/wvffle/waffsh

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
- [x] Escape special characters (`>`, `&`, `#`)
- [x] Add `cd`, `exit` builtins
- [ ] Support redirects in background when `&` is a second token (`ls > ls.txt &`)
- [ ] Support quotes (`grep -E '"'`, `grep -E '"'`, `grep -E '\''`)

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
