# Apollo

Hello everyone, I'm Noah, I live an belgium and I'm 14 years old.
I've always wanted to make my own compiler from scratch. Of course this is very difficult
but I'm just going to try and we will see what happens.


My idea is to make a C like programming language. The compiler will be written in C.


!! IMPORTANT !! The language is in a very early stage of development, this means the language
is basicly useless at the moment. I would say wait a year and come back, then I hope to have 
a functional compiler.


# Compilation

## The compiler itself
You will need to compile the compiler yourself. This is not that hard.

If you have clang installed, you can just run the command below main directory.

```bash
make
```

If you don't have clang installed you need to install it first, for Debian-based linux distros you can type the command below in the terminal to install clang.
```bash
sudo apt install clang
```

for Arch-based distros
```bash
sudo pacman -S clang
```

## Compiling your own apollo source files
To compile your own apollo source files, you need to have the apollo compiler executable. If you don't
know how to compile it, check out the section above this one 'The compiler itself'.

Now to compiler your own files, this command in the terminal.
```bash
./bin/apollo filepath.apo 
```

Ofcourse there are flags you can enable. To get some help with all the flags you can always just run this.
```bash
./bin/apollo -h
```

This will give you some information about the compiler.

## Compiler flags
The compiler has flags you can enable, here is a list with the available flags.

- Flags
  -     -h          Prints a little help message.
  -     -v          Verbose output (while assembling and linking the file).
  -     -asm        Only generates the assembly.
  -     -mac-arm64  Generates code for arm64 (m1) with macOS.
  -     -linux-x64  Generates code for intel x64 with Linux.


# The language
Now I'm going to talk about the syntax of the language.

To define a function.
```
func name() {
    <statement>
    <statement>
    <statement>
    ...
}
```

To call a function.
```
name();
```

One thing to notice, you need to write an entry-point for the program, like in C. You basicly just define a function called `main`.

```
func main() {

}
```

Every program should have an entry point like the example above.


# Goals

- Platforms
  - Linux
  - macOS (arm64)
  - Windows

- Architectures
  - x64
  - aarch64 (arm64)