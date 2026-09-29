#pragma once
#include "graph.h"
#include <math.h>

#define INF 999999.0


// A* Search oVer the topoLogical graph -WE have used A* algorithm to find the shortest distance btw two nodes 
// this file contains the logic of A* alfgorithm like how our system will find its path


// Euclideaan-distance heuristic between two nodes. Requires nodeX[]/nodeY[]
// in graph.h to hold real coordinates for the heuristic to be admissible....
float heuristic(int a, int b) {
  float dx = nodeX[a] - nodeX[b];
  float dy = nodeY[a] - nodeY[b];
  return sqrt(dx * dx + dy * dy);
}

// A* from startNode to goalNode. Same array-based approach 
// right now we have used simple array grid .. queue base implementation
// can can be done further .. 
int astarPath(int startNode, int goalNode, int pathOut[NUM_NODES]) {
  float gScore[NUM_NODES], fScore[NUM_NODES];
  int cameFrom[NUM_NODES];
  bool inOpenSet[NUM_NODES], visited[NUM_NODES];

  for (int i = 0; i < NUM_NODES; i++) {
    gScore[i] = INF;
    fScore[i] = INF;
    cameFrom[i] = -1;
    inOpenSet[i] = false;
    visited[i] = false;
  }

  gScore[startNode] = 0;
  fScore[startNode] = heuristic(startNode, goalNode);
  inOpenSet[startNode] = true;

  for (int count = 0; count < NUM_NODES; count++) {
    int current = -1;
    float best = INF;
    for (int i = 0; i < NUM_NODES; i++) {
      if (inOpenSet[i] && !visited[i] && fScore[i] < best) {
        best = fScore[i];
        current = i;
      }
    }
    if (current == -1) break;      // open set exhausted
    if (current == goalNode) break; // reached goal

    inOpenSet[current] = false;
    visited[current] = true;

    for (int neighbor = 0; neighbor < NUM_NODES; neighbor++) {
      float w = edgeWeight[current][neighbor];
      if (w > 0 && !visited[neighbor]) {
        float tentativeG = gScore[current] + w;
        if (tentativeG < gScore[neighbor]) {
          cameFrom[neighbor] = current;
          gScore[neighbor] = tentativeG;
          fScore[neighbor] = tentativeG + heuristic(neighbor, goalNode);
          inOpenSet[neighbor] = true;
        }
      }
    }
  }

  if (gScore[goalNode] >= INF) return 0; // unreachable

  int tempPath[NUM_NODES];
  int len = 0;
  int cur = goalNode;
  while (cur != -1) {
    tempPath[len++] = cur;
    cur = cameFrom[cur];
  }
  for (int i = 0; i < len; i++) pathOut[i] = tempPath[len - 1 - i];
  return len;
}


void blockEdge(int nodeA, int nodeB) {
  edgeWeight[nodeA][nodeB] = 0;
  edgeWeight[nodeB][nodeA] = 0;
}
