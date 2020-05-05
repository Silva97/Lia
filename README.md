![GitHub release (latest by date)](https://img.shields.io/github/v/release/Silva97/Lia)

# Lia
Lia is a assembly-like programming language compiled to
[Ases](https://github.com/Silva97/Ases) esoteric language.

Lia is extensible! It is very easy to modify your compiler to inserts new instructions.
And if you don't want this, no problem! It is possible to extend Lia in your source code.

```
[new add x:r y:r = "XaY4Ax"]

add rc, rd
```

It is very easy to create new commands to Lia if you knowns Ases. But no problem if you don't want to program in Ases, Lia has modules that you can import and get some commands.  
The main module is 'lia', and you have some basic commands like thats:

```
[import "lia"]

say "Please, input the 'r' character: "
in rc

iequ rc, 'r'
ifnz say "It's not the 'r' character!\n"
```

## Instalation
To install Lia in your system, just run:

```
$ make
$ sudo make install
```

For see help about usage, run `lia -h`.

## Hello World in one command
```
$ echo 'say "Hello World!\n"' | lia -o- - | ases
```