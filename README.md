# CodeBreaker

A Bulls and Cows-style console game written in C. Guess a secret sequence of unique digits using feedback after each attempt.

## Game rules

Choose a difficulty: **Easy** uses 3 digits, **Medium** uses 4 and **Hard** uses 5. Digits range from 0 to 9 and cannot repeat. A sequence may start with zero.

- **Hit:** a correct digit in the correct position.
- **Near Hit:** a correct digit in a different position.

For example, if the secret is `123` and the guess is `132`, the result is **1 Hit, 2 Near Hits**. The game ends when every digit is in the correct position.

## Features

- Checks guess length, digit characters and duplicate digits.
- Saves progress after each guess and can resume a saved game.
- Records completed games and displays the five lowest attempt counts for each difficulty.
- Provides separate menus for starting, loading and viewing scores.

## Build and run

Use Windows with Visual Studio or Visual Studio Build Tools and the **Desktop development with C++** workload. The source uses Microsoft C runtime functions such as `scanf_s`, `fopen_s` and `strcpy_s`.

In a Developer Command Prompt, from the repository directory:

```bat
cl /W4 /TC Source.c /Fe:codebreaker.exe
codebreaker.exe
```

You can also add `Source.c` to an empty Visual Studio console project and build it as C. Enter your name, choose **New Game**, select a difficulty and type guesses without spaces.

## Files and code

| File | Purpose |
| --- | --- |
| `Source.c` | Menus, guess validation, scoring, saved games and high scores |
| `save_game.txt` | Generated progress file; removed when the game is won |
| `highscores.txt` | Generated record of completed games |

`GameState` holds the secret, previous guess, attempt count and most recent result. `checkGuess()` counts exact matches first, then checks unmatched digits for near hits.

## Current scope

This is a single-player study project. Save files are plain text and assume valid local data. The score loader uses fixed-size arrays, and menu input handling needs further work for malformed input or end-of-file. There is no automated test suite yet.

The project exercises arrays, structs, string validation, sorting and file persistence in C.

[More projects](https://github.com/omerbenguigui)
