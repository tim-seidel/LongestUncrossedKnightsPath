# Longest uncrossed closed knight's path algorithm

The longest uncrossed closed knight's path is a mathematical problem involving a knight on the standard NxM chessboard. The problem is to find the longest path the knight can take on the given board, such that the path does not intersect itself. In addition, the path must be closed. Thus it ends at the same place where it begins.

This was a project in the module "Parallel and distributed algorithms" at the University of Applied Sciences in Osnabrueck.

General approach:
- Optimize start fields and moves by tacking symmetries into account.
- Run the algorithm for all start configurations that are relvant.
- Use backtracking to run over all path and move possibilities.
- Store the data in a chessboard that contains all information. A field contains the binary information for all possible 8 moves (8 bits total). Each bit means 1: move is used 0: move is not used. For cross detection of a new move all relevant neighbours are checked whether they contain moves that would lead to a crossing path. This is done by bitoperations with the field data. For more information about the moves and relevant neightbours check the doc/ folder. This leads to an overall fast and optimized calculation. Reference time for the fastest 8x8 chessboard time: ~9.6 seconds.

__sequential__

Sequential implementation of the problem.

__parallel-for__

Parallel implementation using intels tbb::parallel_for approach.

__parallel-tasks__

Parallel implementation using intels tbb::task approach.


