#include "graph.h"
#include "astar.h"
#include "sensors_stub.h"
#include "state_machine.h"
#include "web_server.h"

NavigationFSM nav;

unsigned long lastTick = 0;
const unsigned long TICK_MS = 50;
 // 20Hz control loop......


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("TopoBin-AMR firmware (A*, EMA filter, async web server, no hardware yet)");

  setupWebServer("TopoBin-AMR", "warehouse123"); // TODO: pick a real AP password before demo

  nav.currentNode = NODE_HOME;
  // No task auto-startgs anymore the robot wais for a real /task request
  // from the web UI, matchingd how it will actually be operated....
}

void loop() {
  // Non-blocking: the async server handles requests via its own task

  int newGoal;
  if (pollForNewTask(newGoal)) {
    Serial.print("[Main] New task received: ");
    Serial.println(nodeNames[newGoal]);
    nav.startTask(newGoal);
  }


  if (millis() - lastTick >= TICK_MS) {
    lastTick = millis();
    nav.update();
  }
}
