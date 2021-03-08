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

This implementation that uses parallelizaton approache with the tbb tasks.

# File overwiew
__main.cpp__

The main file parses the command line arguments, runs the algorithm, measures the time and stores the longest path in a file.

__Chessboard__

The logic file that holds the chessboard state and contains the logic for path and crossing detection.

__LUKPStartOptimzation__

The file that includes the calculation of the start configurations (field + move) where algorithm can start the backtrackings. It incudes symmetrie optimizations.

__LUKPData__

Contains data classes to pass the data to the parallel instances and vice versa.

__LUKPTask__

The file that implements one backtracking steps, that each start new child tasks for the next interation.

__LongestUncrossedKnightsPath__

The file that contains the controlflow of the algorihm. It creates the chessboard in the desired configuration. It calculates the start configurations provided by LIKPStartOptimization.  The algorihm gets with a root task for each start(field + move).