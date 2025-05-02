# Tic-Tac-Toe (XO) Bot Game

A C implementation of a Tic-Tac-Toe game featuring:
- Console Mode: Play in the terminal with 1-based coordinates; tracks all moves in a hashmap and stores board history in a binary tree.
- GUI/Web Mode: A graphical version using Raylib, compiled to WebAssembly for play in the browser.

IMPORTANT (**  **, have this around them)

---

## 📂 Repository Structure

/
├── console/  
│   ├── Project.c           # Terminal version (hashmap + tree)  
│   ├── uthash.h            # Hashmap header (included)  
│
├── gui/  
│   ├── main.c              # Raylib GUI version  
│   ├── Makefile            # Emscripten build rules  
│   ├── index.html          # Web page loader  
│   ├── index.js            # Emscripten runtime glue  
│   └── index.wasm          # Compiled WebAssembly  
│
└── README.txt              # ← You are here

---

## 🛠 Prerequisites

### Console Version
- A C compiler (e.g., gcc)  
- ** Use Mingw64 Compiler (Raylib , WebAssembly use it) **
- Standard C library  
- No external libs (just include uthash.h provided)  

### GUI/Web Version
- Raylib installed on your system for desktop builds  
- Emscripten SDK (emcc, emmake) for WebAssembly builds  
- Python 3 (for python -m http.server)  

---

## ⚙️ Building & Running

### 1. Console Mode

1. Navigate into the console directory:
   cd console
2. Compile:
   gcc Project.c -o tic_tac_toe_console
3. Run:
   ./tic_tac_toe_console

4. ** HOW IT WORKS : **  
   - Choose X or O and difficulty (E/M/H).  
   - Enter moves as row col (both 1–3).  
   - All moves are stored in a hashmap (uthash.h) and the full board history in a tree.  
   - At game end it prints move lists and side-by-side board states.

### 2. GUI & Web Mode

#### A. Desktop (Raylib)

1. Navigate into GUI folder:
   cd gui
2. Modify the build command for your setup. For example, on Windows:
   gcc main.c -o tic_tac_toe_gui        -I"<YOUR_RAYLIB_INCLUDE_PATH>"        -L"<YOUR_RAYLIB_LIB_PATH>" -lraylib -lopengl32 -lgdi32 -lwinmm
3. Run:
   ./tic_tac_toe_gui
   The window opens at 900×900; click to place moves.

#### B. WebAssembly (Browser)

1. Install / Activate Emscripten SDK, then:
   cd gui
   emmake make
2. Serve locally:
   python -m http.server 8000
3. Open your browser at http://localhost:8000 to play.  

4. ** THE WEBSITE IS ONLINE AT : **  
   https://lleclipsell.github.io/Tic_Tac_Toe/

---

## 🎮 Usage

- Console:  
  - Input coordinates like 2 3 to place at row 2, col 3.  
  - Watch printed histories.

- GUI/Web:  
  - Choose X or O, select difficulty via on-screen buttons.  
  - Click on cells to play.  
  - Restart via the “Restart” button after a win/draw.

---

## 📝 Contributing

Feel free to open issues or pull requests for:
- Bug fixes  
- New difficulty levels or AI improvements  
- UI enhancements  

---

## 📜 License

This project is released under the MIT License. See LICENSE for details.
