# sutrix
## Preview

Falling columns of katakana (75%), symbols (15%), and numbers (10%), fading out as they age.(as cmatrix) Three color modes(easy to swap), adjustable speed, all controlled live without restarting.

## Build

Requires `gcc` and `ncursesw` (wide-character ncurses).

```sh
sudo apt install libncursesw5-dev   # Debian based
sudo pacman -S ncurses        # Arch based
```

Then build and install to `~/.local/bin` (no doas or sudo perms required)

``` 
gcc -O2 -o sutrix sutrix.c -lncursesw && chmod +x sutrix && mkdir -p ~/.local/bin && cp sutrix ~/.local/bin/
```

Make sure `~/.local/bin` is on your `PATH`. If not, add it to your shell's startup file:

```sh
# bash: ~/.bashrc | zsh: ~/.zshrc | dash/sh (e.g. Chimera default): ~/.profile
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
. ~/.profile
```

## Error if path fails to 
```sh

mkdir -p ~/.local/bin
gcc -O2 -o ~/.local/bin/sutrix sutrix.c -lncursesw
chmod +x ~/.local/bin/sutrix

```


## Run

```sh
sutrix
```

thanks sunia for the code <3

## License
gplv2
