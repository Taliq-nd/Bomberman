# ASCII Bomberman

A 2-player Bomberman clone written in C with Raylib.

## Controls

| Action     | Player 1     | Player 2 |
|------------|-------------|----------|
| Move Up    | ↑           | W        |
| Move Down  | ↓           | S        |
| Move Left  | ←           | A        |
| Move Right | →           | D        |
| Place Bomb | SPACE       | F        |

## Power-ups

| Icon  | Effect           |
|-------|-----------------|
| +R (Yellow) | Increases bomb explosion range |
| +S (Blue)   | Increases movement speed       |
| +B (Purple) | Adds one extra bomb slot       |

## Features

- 2-player local multiplayer
- Best of 3 rounds match system (first to 3 wins)
- Safe enemy spawning (enemies can't spawn near players)
- Random enemy movement with variable speed
- Chain reaction bombs
- Sudden death: walls close in after 90 seconds
- Particle effects on explosions
- Player invulnerability after being hit (1.5s)
- Power-up drops from destroyed blocks (33% chance)

## Build

### Windows (MSYS2 + GCC)

```powershell
# Build only
.\make.ps1

# Build and run
.\test.ps1
```

Or manually:
```cmd
gcc main.c src\game.c src\map.c src\player.c src\bomb.c src\enemy.c src\render.c -o bomberman.exe -Isrc -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -lraylib -lopengl32 -lgdi32 -lwinmm
```

### MSYS2 / Linux / macOS

```bash
bash make.sh   # build
bash test.sh   # build + run
```

## Project structure

```
bomberman/
├── src/
│   ├── types.h     # All shared structs and constants
│   ├── main.c      # Window init and main loop
│   ├── game.c/.h   # Game state and round/match logic
│   ├── map.c/.h    # Grid, tiles, sudden death spiral
│   ├── player.c/.h # Player movement, bombs, power-ups
│   ├── bomb.c/.h   # Bomb placement, explosion, chain reaction
│   ├── enemy.c/.h  # Enemy AI and safe spawn
│   └── render.c/.h # All drawing + particles
├── assets/
├── make.ps1        # Windows build script
├── test.ps1        # Windows build + run
├── make.sh         # Linux/MSYS2 build script
├── test.sh         # Linux/MSYS2 build + run
├── CMakeLists.txt
└── README.md
```
