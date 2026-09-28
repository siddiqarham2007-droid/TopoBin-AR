#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>   // library: ESPAsyncWebServer (+ its AsyncTCP dependency)
#include "graph.h"


// Asynchronous task-assignment web server.



AsyncWebServer server(80);

volatile int pendingGoalNode = -1;
volatile bool hasPendingTask = false;

void setupWebServer(const char* apSSID, const char* apPassword) {
  WiFi.softAP(apSSID, apPassword);
  Serial.print("[WebServer] AP started, IP: ");
  Serial.println(WiFi.softAPIP());

  // GET /task    eg- /task?goal=4 for DispatchB
  server.on("/task", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("goal")) {
      request->send(400, "text/plain", "Missing 'goal' parameter");
      return;
    }
    int goal = request->getParam("goal")->value().toInt();
    if (goal < 0 || goal >= NUM_NODES) {
      request->send(400, "text/plain", "Invalid node id");
      return;
    }
    pendingGoalNode = goal;
    hasPendingTask = true;
    request->send(200, "text/plain", "Task accepted: " + String(nodeNames[goal]));
  });

  // TODO:will swap this route for the real embedded web UI (station buttons)
 

  server.begin();
}

bool pollForNewTask(int &goalOut) {
  if (hasPendingTask) {
    goalOut = pendingGoalNode;
    hasPendingTask = false;
    return true;
  }
  return false;
}
