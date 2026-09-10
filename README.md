# Tanks Game

A real-time tank battle for the Windows console, written in C++17.
Two players (or the computer) drive tanks track by track, aim 8-way cannons, and fight across maps with breakable walls, mines and wrap-around tunnels.
Every game can be **recorded and replayed deterministically**, which also powers a headless regression-test mode.

## Gameplay

- **Tread steering** — each tank has a left and a right track: both forward drives straight, one track rotates the cannon, both backward reverses
- **8-direction cannon** — hit the enemy's body to destroy it, hit the cannon to disarm it
- **Maps from files** — walls (`#`), mines (`@`), tank start positions (`1`, `2`) and a score box (`L`) are read from `tanks-game_*.screen` files; levels play in order
- **Tunnels** — the map edges wrap around for tanks and shells alike
- **Breakable walls** — a wall (`#`) cracks (`%`) on the first hit and falls on the second
- **Game modes** — Human vs Human, Human vs Computer, Computer vs Computer, with several tanks per player
- **Color mode** and an in-game pause menu (`ESC`)

### Controls

| Action | Player 1 | Player 2 |
|---|:---:|:---:|
| Left track forward / back | `Q` / `A` | `U` / `J` |
| Right track forward / back | `E` / `D` | `O` / `L` |
| Stop | `S` | `K` |
| Shoot | `W` | `I` |
| Switch tank | `Z` | `M` |

## Record, replay and silent tests

```
tanksGame_v2.exe                # normal game
tanksGame_v2.exe -save          # play and record every screen to .steps + .result
tanksGame_v2.exe -load          # replay the recorded games on screen
tanksGame_v2.exe -load -silent  # replay without drawing and check the outcome
```

A recording stores the random seed, the game mode and every key pressed on every tick (`.steps`).
Because the simulation is driven only by the seed and the input, replaying it reproduces the game exactly.
`-save` also writes the events of the game — hits, mines, final score — to a `.result` file;
`-load -silent` re-runs the game headless, rebuilds that event list, and reports `Test PASSED` / `Test FAILED` with the first differing lines.

## Computer opponent

Each computer tank, every tick:
1. scans all 8 firing lines (following the tunnels) and shoots when an enemy body or cannon is in line of sight, and no wall is in between
2. otherwise checks whether a shell is heading toward it within 10 steps and drives out of its path

## Code structure

| File | Responsibility |
|---|---|
| `game.*` | menus, level loading, main loop, shell collisions, scoring, record/replay |
| `player.h` | abstract `Player` owning its tanks |
| `humanPlayer.*`, `computerPlayer.*` | keyboard-driven player and AI player |
| `tank.*` | movement, tread steering, cannon rotation, drawing |
| `shell.*`, `point.*` | projectiles and 2D coordinates on a wrapping board |
| `steps.*`, `result.*` | recording format and expected-results format |
| `gameConfig.h`, `general.*` | board size, keys, console helpers |

## Build

Windows + Visual Studio 2022 (C++17). Open `tanksGame_v2.sln` and build, or run the exe from the folder that holds the `.screen` files.

## Version history

The game was developed in stages between April and June 2025:

- **v1** (April) — tanks, tread steering, shells, walls and mines, color mode
- **v2** (May) — maps loaded from `.screen` files, players owning several tanks
- **v3** (June) — human and computer players, then deterministic record/replay and the silent test mode
- **v3.5** (this version) — replay and test-mode fixes: the silent test now compares the replay's own results, both players' input (including tank switching and player 2's shots) is replayed in order, player 2 moves at the same speed as player 1, and destroyed tanks are freed
