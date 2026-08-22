## Build

Requires `gcc` and `ncursesw` (wide-character ncurses).

```
sudo apt install libncursesw5-dev   # Debian based
sudo pacman -S ncurses        # Arch based
```

Then build and install to `~/.local/bin` (no doas or sudo perms required)

``` 
gcc -O2 -o sutrix sutrix.c -lncursesw && chmod +x sutrix && mkdir -p ~/.local/bin && cp sutrix ~/.local/bin/
```

Make sure `~/.local/bin` is on your `PATH`. If not, add it to your shell's startup file:

```
# bash: ~/.bashrc | zsh: ~/.zshrc | dash/sh (e.g. Chimera default): ~/.profile
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
. ~/.profile
```

## Fonts if for some reason you dont have it installed 
```
Sarasa Term J
```


## Error if path fails to 
```

mkdir -p ~/.local/bin
gcc -O2 -o ~/.local/bin/sutrix sutrix.c -lncursesw
chmod +x ~/.local/bin/sutrix

```


## Run

```
sutrix
```

thanks sunia for the code <3

## License
gplv2
