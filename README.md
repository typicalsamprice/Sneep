[comment]: # (-*- compile-command: "pandoc README.md -o Proposal.pdf -V colorlinks=true -V linkcolor=blue" -*-)

# 2023-24 Midterm (Extended to Final) Project (Intermediate C++ - COMPSCI 222)

This aims to be a functional chess engine, like [Stockfish](https://github.com/official-stockfish/Stockfish), [Inanis](https://github.com/Tearth/Inanis)
or [Sebastian Lague's Engine](https://github.com/SebLague/Chess-Challenge). This will not be *nearly* as fully-featured, nor as competent.

Lots of "inspiration" (where cited) is taken from Stockfish, with much other information coming from my past attempts at engines
as well as the [Chess Programming Wiki](https://www.chessprogramming.org).

The engine and its history will live at [typicalsamprice/Sneep](https://github.com/typicalsamprice/Sneep), with an attempt at coherent
commit messages.

**Note: This Project is compiled with C++20 to take advantage of STL functions that deal with bit operations**

## Primary Goals
- [ ] Recieve FEN from user via CLI
  - `position fen FEN_STRING` will set the position to said FEN, verifying it is valid.
- [ ] Properly initialize `Position` and `State` structs to properly store game information. 
- [ ] Generate moves
  - [ ] Validate moves, using [perft](https://www.chessprogramming.org/Perft), short for "performance testing".

## Stretch Goals
When I inevitably do not succeed in some (or all) of these goals, they will become the goals for the Final Project,
along with a few others I can think of now.

- [ ] Detect checkmate and react accordingly
  - Sounds stupid, I don't care if there's no game loop, it won't matter.
- [ ] Use [Magic Bitboards](https://www.chessprogramming.org/Magic_Bitboards) rather than traditional slider generation
  - Also discussed [here](https://analog-hors.github.io/site/magic-bitboards/), where I got my less-technical but better founded introduction.
    Fun Fact! This site uses Rust, which is what the vast majority of my attempts at this nonsense have been written in.
    It's much nicer for many, many reasons. However, I both do not have a choice here AND for this application specifically
    there are a few caveats Rust doesn't play too nicely with unless you want a headache (nested/potentially recursive structures.)
- [ ] Allow user input to describe moves to be made
- [ ] Make rudimentary opponent instead of user playing both sides
  - [ ] Possibly using an [alpha-beta search](https://www.chessprogramming.org/Alpha-Beta), or simply minimax if I'm lazy
    - Also highly likely to simply pick a random move with no evaluation, and leave the more complex work for the final.
