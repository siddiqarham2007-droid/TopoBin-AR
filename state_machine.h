#pragma once
#include "graph.h"
#include "astar.h"         
#include "sensors_stub.h"

enum RobotState {
  STATE_IDLE,
  STATE_PLANNING,
  STATE_MOVING,
  STATE_ARRIVING,
  STATE_SEARCHING,      
  STATE_OBSTACLE_WAIT,
  STATE_REPLANNING,
  STATE_LOST,
  STATE_TASK_COMPLETE
};

class NavigationFSM {
  public:
    RobotState state = STATE_IDLE;
    int path[NUM_NODES];
    int pathLength = 0;
    int pathIndex = 0;
    int currentNode = NODE_HOME;
    int goalNode = NODE_HOME;
    unsigned long obstacleTimerStart = 0;
    const unsigned long OBSTACLE_WAIT_MS = 5000;
    const float SEARCH_MAX_CM = 40.0; // give up creeping after this much extra travel

    void startTask(int goal) {
      goalNode = goal;
      state = STATE_PLANNING;
    }

    void update() {
      switch (state) {

        case STATE_IDLE:
          break;

        case STATE_PLANNING: {
          pathLength = astarPath(currentNode, goalNode, path);
          if (pathLength == 0) {
            Serial.println("[FSM] No path found to goal.");
            state = STATE_IDLE;
          } else {
            pathIndex = 1;
            Serial.print("[FSM] Path planned: ");
            for (int i = 0; i < pathLength; i++) {
              Serial.print(nodeNames[path[i]]);
              if (i < pathLength - 1) Serial.print(" -> ");
            }
            Serial.println();
            state = STATE_MOVING;
          }
          break;
        }

        case STATE_MOVING:
          driveTowardsNode(path[pathIndex]);
          updateOdometry();

          if (obstacleDetected()) {
            Serial.println("[FSM] Obstacle detected, stopping.");
            stopMotors();
            obstacleTimerStart = millis();
            state = STATE_OBSTACLE_WAIT;
          } else if (odometryEstimatesArrival(currentNode, path[pathIndex])) {
            state = STATE_ARRIVING;
          } else if (odometryPastExpectedDistance(currentNode, path[pathIndex])) {

            Serial.println("[FSM] Past expected distance without a tag hit - slowing to search.");
            state = STATE_SEARCHING;
          }
          break;

        case STATE_ARRIVING: {
          stopMotors();
          int confirmedNode;
          if (readRFID(confirmedNode) && confirmedNode == path[pathIndex]) {
            Serial.print("[FSM] RFID confirmed node: ");
            Serial.println(nodeNames[confirmedNode]);
            resetOdometryOrigin(confirmedNode);
            currentNode = confirmedNode;
            pathIndex++;
            state = (currentNode == goalNode) ? STATE_TASK_COMPLETE : STATE_MOVING;
          } else {
/*will do ome more research before throwmg an error*/
            Serial.println("[FSM] Expected tag not read at arrival - entering search.");
            state = STATE_SEARCHING;
          }
          break;
        }

        case STATE_SEARCHING: {
          /*move slow and searcch for the tAG */
          driveSlow();
          int confirmedNode;
          if (readRFID(confirmedNode) && confirmedNode == path[pathIndex]) {
            Serial.println("[FSM] Tag found during recovery search.");
            resetOdometryOrigin(confirmedNode);
            currentNode = confirmedNode;
            pathIndex++;
            state = (currentNode == goalNode) ? STATE_TASK_COMPLETE : STATE_MOVING;
          } else if (odometryPastExpectedDistance(currentNode, path[pathIndex], SEARCH_MAX_CM)) {
            Serial.println("[FSM] Search window exhausted - entering LOST.");
            stopMotors();
            state = STATE_LOST;
          }
          break;
        }

        case STATE_OBSTACLE_WAIT:
          if (!obstacleDetected()) {
            Serial.println("[FSM] Obstacle cleared, resuming.");
            state = STATE_MOVING;
          } else if (millis() - obstacleTimerStart > OBSTACLE_WAIT_MS) {
            Serial.println("[FSM] Obstacle persists, replanning around blocked edge.");
            blockEdge(currentNode, path[pathIndex]);
            state = STATE_REPLANNING;
          }
          break;

        case STATE_REPLANNING:
          state = STATE_PLANNING; // re-run A* rom currntNode with the edge blocked
          break;

        case STATE_LOST:
          // TODO once hardware completes: reverse to currentNode's last known
          // tag and halt for operator intervention. Safe default for now.
          stopMotors();
          Serial.println("[FSM] LOST - halted, awaiting recovery logic.");
          break;

        case STATE_TASK_COMPLETE:
          Serial.println("[FSM] Task complete.");
          state = STATE_IDLE;
          break;
      }
    }
};
