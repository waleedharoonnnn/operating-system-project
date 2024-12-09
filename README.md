
A* Search Algorithm with Multithreading

This project implements the A* search algorithm in C++ with a multithreading approach to optimize the pathfinding process. The algorithm is used to find the shortest path in a grid-based map from a given source to a destination.


Key Features: Multithreading, Pathfinding Algorithm

Features
Multithreading: Eight threads handle successors of the current cell to enhance performance.
Customizable Grid: Modify the grid array to define blocked and unblocked cells.
Interactive Display: Displays the grid, source (S), and destination (D) points in a user-friendly format.
Heuristic Calculation: Uses the Euclidean distance to estimate the cost to the destination.


How It Works

Grid Definition: A grid is defined with 1 for unblocked cells and 0 for blocked cells.
Pathfinding: The algorithm evaluates neighboring cells and calculates the f = g + h value:
g: Cost from the source to the current cell.
h: Estimated cost from the current cell to the destination.
Multithreading: Threads evaluate the 8 possible successors (N, S, E, W, NE, NW, SE, SW) concurrently.
Output: If a path is found, the sequence of cells is printed; otherwise, a failure message is displayed.
