# cstat
A simple small program for getting some infos on C/C++ files, inspired [cloc](https://github.com/AlDanial/cloc).

## Usage

```
cstat ./path/to/src
```

```
--------------------------------------------------------------------------------
Language        Files          Blanks        Comments                      Lines
--------------------------------------------------------------------------------
C/C++               1              21               2                        167
--------------------------------------------------------------------------------
Sum                 1              21               2                        167
```

## Compilation
This project was setup using VSCode. The build task is included in [.vscode](.vscode) using cl. Make sure you have Visual Studio installed and hit ctrl + shift + B to build cstat.

## Motivation
I just wanted a small side project to play around with the [Filesystem library](https://en.cppreference.com/w/cpp/filesystem) of C++17. Nothing more, nothing less.