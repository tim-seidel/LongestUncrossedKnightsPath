# Usage
__Run commands:__
Zum einfachen Erstellen der ausführbaren Datei steht das Skript ./make_runfile zur Verfügung (ggf. muss chmod +x ausgeführt werden.)
Danach kann die Datei einfach gestartet werden:
```bash
source /opt/intel/tbb2019_20181203oss/bin/tbbvars.sh intel64
./path -m 8 -n 8 (-t 2)
```
Zum manuellen compileren kann folgender Befehl verwendet werden:
```bash
g++ *.cpp -o path -O3 -ltbb
```

Die Ausführung erfolgt mit:
```bash
source /opt/intel/tbb2019_20181203oss/bin/tbbvars.sh intel64
./path -m 8 -n 8 (-t 2)
```

# Longest uncrossed closed knight's path algorithm

The longest uncrossed closed knight's path is a mathematical problem involving a knight on the standard NxM chessboard. The problem is to find the longest path the knight can take on the given board, such that the path does not intersect itself. In addition, the path must be closed. Thus it ends at the same place where it begins.

This implementation uses that parallelizaton approache with the tbb::parallel_for loop.

# File overwiew
__main.cpp__

The main file parses the command line arguments, runs the algorithm, measures the time and stores the longest path in a file.

__Chessboard__

The logic file that holds the chessboard state and contains the logic for path and crossing detection.

__LUKPStartOptimzation__

The file that includes the calculation of the start configurations (field + move) where algorithm can start the backtrackings. It incudes symmetrie optimizations.

__LUKPData__

Contains data classes to pass the data to the parallel instances and vice versa.

__LUKPIterator__

The file that implements the neccessary code for the parallel_for loop. Its starts the backtracking from every start configuration calculated by LUKPStartOptimization.

__LongestUncrossedKnightsPath__

The file that contains the controlflow of the algorihm. It creates the chessboard in the desired configuration. It calculates the start configurations provided by LIKPStartOptimization. It can limit the threads and calculate the amount of tasks to balance the for loop parallelization. (Too few tasks may leed to differnt calculation time between the threads). The algorihm gets started with a parallel_for loop implementaiton that is feeded with arrays containing the start configurations and objects to store and retreive the results for each parallel instance. After the calculation the results get accumulated and returned.
