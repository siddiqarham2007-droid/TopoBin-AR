#pragma once
#include <Arduino.h>

// Warehouse topological graph
// Nodes represent junctions/stations only (not every physical point) -


// PLACEHOLDER DATA: update NUM_NODES, node names, edgeWeight[][], and
// nodeX[]/nodeY[] 

#define NUM_NODES 6

enum NodeID {
  NODE_HOME = 0,
  NODE_J1,
  NODE_J2,
  NODE_PICKUP_A,
  NODE_DISPATCH_B,
  NODE_J3
};

const char* nodeNames[NUM_NODES] = {
  "Home", "J1", "J2", "PickupA", "DispatchB", "J3"
};

// Adjacency matrix of edge weights in centimetres. (0 = no direct edge.)

float edgeWeight[NUM_NODES][NUM_NODES] = {
  /*              Home   J1     J2     PickupA  DispatchB  J3   */
  /* Home       */{0,     120,   0,     0,        0,          0},
  /* J1         */{120,   0,     100,   80,       0,          0},
  /* J2         */{0,     100,   0,     0,        90,         70},
  /* PickupA   */{0,     80,    0,     0,        0,          0},
  /* DispatchB */{0,     0,     90,    0,        0,          0},
  /* J3         */{0,     0,     70,    0,        0,          0}
};


float nodeX[NUM_NODES] = {   0, 120, 220, 120, 220, 290 };
float nodeY[NUM_NODES] = {   0,   0,   0, -80,  90,   0 };
