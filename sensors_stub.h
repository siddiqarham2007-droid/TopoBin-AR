#pragma once
#include <Arduino.h>
#include "graph.h"

// ---------------------------------------------------------------
// HARDWARE STUB LAYER
//
/*
 * NOTE:
 * This is a preliminary prototype implementation.
 * Further modifications and integration will be made based on
 * the specific requirements of the final robot.
 */

float traveledDistance = 0; // placeholder odometry accumulator

void driveTowardsNode(int targetNode) {
  // TODO: compute heading to targetNode and set motor driver PWM/direction.
  traveledDistance += 5.0; // placeholder: pretend we moved 5cm this tick
}

void driveSlow() {
  // TODO: same as driveTowardsNode() but at reduced PWM used during the
  // missing-tag recovery search so the robot creeps rather than barrels
  // past a tag it's struggling to read.
  traveledDistance += 2.0; // slower placeholder increment
}

void stopMotors() {
  // TODO: will set motor driver PWM to 0 on both channels.
}

void updateOdometry() {
  // TODO: it will read wheel encoder ticks + IMU heading, integrate into an
  // internal (x, y, theta) pose estimate.
}

bool odometryEstimatesArrival(int fromNode, int targetNode) {
  
  bool arrived = traveledDistance >= edgeWeight[fromNode][targetNode];
  if (arrived) traveledDistance = 0;
  return arrived;
}
// Missing-tag recovery trigger:
bool odometryPastExpectedDistance(int fromNode, int targetNode, float marginCM = 15.0) {
  // if we've traveled past the expected edge
  // length (plus a margin for normal odometry error) and still haven't
  // matched a tag, something has been missed hand off to the FSM's
  // SEARCHING state rather than continuing to drive blind.
  return traveledDistance > (edgeWeight[fromNode][targetNode] + marginCM);
}

void resetOdometryOrigin(int confirmedNode) {
  // TODO:  will snap the internal (x, y) pose estimate to confirmedNode's known
  // map coordinates (nodeX[confirmedNode], nodeY[confirmedNode]).
  traveledDistance = 0;
}

bool readRFID(int &nodeIdOut) {
  // TODO:  will replace with an actual MFRC522 read (SPI) plus a tag-UID-to-node
  // lookup table.
  nodeIdOut = -1;
  return false;
}

//  Distance sensor: raw read + Exponential Moving Average filter 

float filteredDistanceCM = -1;      
const float EMA_ALPHA = 0.3;  
// -1 = not yet seeded
 // higher = more responsive to new readings,     
 // lower = smoother but slower to react. 0.3
 // is a reasonable starting point — tune once
 // you have real sensor noise to test against.                           

float readRawDistanceCM() {
  // TODO: will replace with actual sensor read (ultrasonic pulse timing, or VL53L0X.
  return 999; // placeholder "clear path" reading
}

float getFilteredDistanceCM() {
  float raw = readRawDistanceCM();
  if (filteredDistanceCM < 0) {
    filteredDistanceCM = raw; // seed the filter on the first real reading
  } else {
    filteredDistanceCM = EMA_ALPHA * raw + (1 - EMA_ALPHA) * filteredDistanceCM;
  }
  return filteredDistanceCM;
}

bool obstacleDetected() {
  // right now we have taken the min dist as 5-6 cm .. further we can update it as per
  // req..
  const float STOP_THRESHOLD_CM = 6.0;
  return getFilteredDistanceCM() < STOP_THRESHOLD_CM;
}
