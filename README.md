# Zeal coreutils

This is a coreutils collection for [Zeal 8-bit Computer](https://zeal8bit.com/).

This collection contains the core utilities for interacting with [Zeal 8-Bit OS](https://github.com/Zeal8bit/Zeal-8-bit-OS/).

**The following utilities are included:**

* cat - output the contents of a file to the terminal
* cp - copy src file to dst file
* crc32 - print the crc32 of each file argument
* du - show size of each directory
* echo - echo the arguments to the terminal
* hexdump - output the contents of a file to the terminal in hex format
* ls - list a directory
* mkdir - make a directory
* mv - move src file to dst file
* rm - remove/delete a file or directory
* setfont - allows you to change terminal fonts
* stat - display file or directory status information
* touch - create an empty file or update timestamps
* tree - show a directory tree

**The following utilities are not currently implemented:**

* dir
* less
* xfer

## Command Documentation

### cat

```text
usage: cat <path>
```

Outputs the contents of a file to the terminal. Reads the entire file and displays it on the console.

### cp

```text
usage: cp <src> <dst>
```

Copy source file to destination file.

### crc32

```text
usage: crc32 <file1> [<file2>] ... [<filen>]
```

Calculate and display the CRC32 checksum of one or more files. Useful for verifying file integrity.

### du

```text
usage: du [path]
```

Display disk usage for directories. Shows the size of each directory and its contents. If no path is specified, uses the current directory.

### echo

```text
usage: echo <string>
```

Print the string argument to the terminal followed by a newline.

### hexdump

```text
usage: hexdump <path>
```

Display the contents of a file in hexadecimal format with ASCII representation. Useful for examining binary files.

### ls

```text
usage: ls [-options] [path]
```

List the contents of a directory. If no path is specified, lists the current directory.

* `l` - list details
* `1` - 1 entry per line
* `x` - hex output
* `k` - kilobytes
* `d` - show date

### mkdir

```text
usage: mkdir [-pv] <path>
```

Create directories.

* `-p` - Create parent directories as needed (recursive creation)
* `-v` - Verbose mode, shows directories being created

### mv

```text
usage: mv <src> <dst>
```

Move or rename files and directories.

### rm

```text
usage: rm [-rfv] <path>
```

Remove files and directories.

* `-r` - Recursive removal (delete directories and their contents)
* `-f` - Force removal (suppress prompts and errors)
* `-v` - Verbose mode, shows what files are being removed

### setfont

```text
usage: setfont <font.f12>
```

Change the terminal font by loading a new font file in F12 format.

### stat

```text
usage: stat [-fd] <path>
```

Display detailed information about a file or directory, including size, type, and timestamps.

* `-f` - Show file information only
* `-d` - Show directory information only

### touch

```text
usage: touch <path>
```

Create an empty file if it doesn't exist, or update the timestamp of an existing file.

### tree

```text
usage: tree [path]
```

Display a directory tree structure showing the hierarchical organization of files and directories. If no path is specified, uses the current directory.

## Installation

Copy these utilities to your ROM Disk, or install them on any other disk.

## Building from source

Make sure that you have [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed.

Then open a terminal, go to the source directory and type the following commands:

```shell
    zde cmake
```
