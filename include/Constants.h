#ifndef PETRINETS_CONSTANTS_H
#define PETRINETS_CONSTANTS_H

// Constants related to graphical representation
const int REACHABILITY_GRAPH_WIDTH = 1000; // The width of the reachability graph
const int ITERATIONS = 500;                // The number of iterations for force-directed graph layout
const double ATTRACTION_CONSTANT = 0.5;    // The attraction constant used in the graph layout algorithm
const double MAX_FORCE = 1e5;              // The maximum force to apply in graph layout
const double MIN_FORCE = 1e-3;             // The minimum force to apply in graph layout
const int NODE_RADIUS = 25;                // The radius of a node in the graph
const int SPACE_BETWEEN_NODES = 200;       // Space between nodes in the SVG output

#endif //PETRINETS_CONSTANTS_H
