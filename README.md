# keepys

Simple password manager for Linux.

- Creates random passwords composed of 16 ASCII characters.
- Passwords are stored in files under `${XDG_DATA_HOME}/keepys` or
  `${HOME}/.local/share/keepys`.
- Files are encrypted with a master password.

Usage:

```
keepys {create|get} <name>
```


## Build

Run `make`.

Dependencies:
- OpenSSL
