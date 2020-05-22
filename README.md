![GitHub release (latest by date)](https://img.shields.io/github/v/release/Silva97/Lia)
[![Build Status](https://travis-ci.com/Silva97/Lia.svg?branch=master)](https://travis-ci.com/Silva97/Lia)

# Lia
Lia is an assembly-like language extensible, and can be compiled to
[Ases](https://github.com/Silva97/Ases) esoteric language.

It is very easy to modify your compiler to inserts new instructions.
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

## Installation
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

If you want to learn how to program in Lia, see the [Wiki here](https://github.com/Silva97/Lia/wiki).
