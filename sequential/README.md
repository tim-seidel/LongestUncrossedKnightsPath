# Usage
__Run commands:__
To compile the project use:
```bash
source /opt/intel/tbb2019_20181203oss/bin/tbbvars.sh intel64
g++ *.cpp -o path -O3 -ltbb
```

To run the project use (the parameter t defines the number of threads and is optional. by default all threads are used.)
```bash
source /opt/intel/tbb2019_20181203oss/bin/tbbvars.sh intel64
./path -m 8 -n 8 (-t 2)
```

# Longest uncrossed closed knight's path algorithm

The longest uncrossed closed knight's path is a mathematical problem involving a knight on the standard NxM chessboard. The problem is to find the longest path the knight can take on the given board, such that the path does not intersect itself. In addition, the path must be closed. Thus it ends at the same place where it begins.

# File overwiew
__main.cpp__

The main file parses the command line arguments, runs the algorithm, measures the time and stores the longest path in a file.

__Chessboard__

The logic file that holds the chessboard state and contains the logic for path and crossing detection.

__LongestUncrossedKnightsPath__

The file that contains the controlflow of the algorihm. It creates the chessboard in the desired configuration. It calculates the start configurations and optimizations. This is the sequential implementations.
