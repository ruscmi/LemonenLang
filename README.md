# ⚠️ The project is under development,version v0.2

== **Russian [README.ru](README.ru.md)** ==

== **Path to [examples_files](examples_code/)** ==
# 🍋 LemonenLang

<p align="center">
	<img src="media/lmnlang_v2b.png" width="100%" alt="lmnlang banner">
</p>

Hello everyone! 
Lmnlang(Lemonenlang) is a fast dynamically typed scripting language with automatic memory management. 

What has been done in it?

- **Lexer**
- **Parser**
- **AST-tree**
- **Interpreter**
- **Runner**
- **Libs**

Im using unordered_map to memory for variables,it works very fast and builds an AST tree almost instantly.

to view commands and learn through manuals, enter the following inside the REPL mod:

```lmnlang
man list
```

or see [manual](src/manual.cpp)

The language is updated daily and I try to work on it alone.

**Installation and compilation.**

== **FOR LINUX** ==
Termux(Android) is also supported
To get started, copy the repository and go to it.

```bash
git clone https://github.com/ruscmi/LemonenLang && cd LemonenLang
```

After that, just run the bash script, which will compile everything you need for you.

```bash
chmod +x builder.sh
./builder.sh
```
== **FOR WINDOWS** ==

For Windows, you will have to sweat. 

best way to install [MSYS2](https://www.msys2.org/)

in terminal `MSYS2 UCRT64` :

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-readline
```

Assemble the project:

```bash
cmake -B build && cmake --build build
```

**Executing files and REPL**

in order to enter the REPL mode (Read Eval Print Loop mode) 
You need to go to the directory where the code was compiled:

```bash
cd LemonenLang
```
If you have a project in another directory, then write the path to it:

ON WINDOWS
```bash
cd path\to\directory
```
ON LINUX
```bash
cd path/to/directory
```
Next, when you are inside LemonenLang, simply write:

ON WINDOWS(powershell,fuck cmd)
```bash
.\build\lmnlang.exe
```
ON LINUX
```bash
./build/lmnlang
```
will ONLY work if the files is compiled

To run files, use the same principle, 
but write the path to the file and specify the --file flag:

ON LINUX
```bash
./build/lmnlang --file path/to/directory
```
ON WINDOWS
```bash
.\build\lmnlang.exe --file path\to\directory
```

But since the v0.2 update, you can run it simply with the command in the terminal:

```bash
lmnlang
```

for open files:

```bash
lmnlang --file /path/to/directory
```

thanks for reading tutorial

# Logo
<p align="center">
	<img src="media/lmnlang_v2.png" width="50%" alt="lmnlang logo">
</p>

made in ibispaint (mobile)

🍋 Goodbye,lemons! 
