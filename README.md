# TopoBin-AMR — firmware

SIH26112 — Intelligent Modular AMR/AGV Platform for Smart and Efficient Warehouse Automation

## Current status

As our physical hardware has not  been completed  yet. This repository currently contains the
**navigation logic** — the topological graph, A* path planning, and the
finite-state machine that ties planning, localization, and obstacle handling
together — fully implemented and testable via the Arduino Serial Monitor.

Sensor and motor I/O (`sensors_stub.h`) is intentionally **stubbed with TODOs**.
This is a deliberate design choice: the FSM's control logic is hardware-agnostic,
so it can be written and verified now, and the stub functions will be swapped for
real RC522 / MPU6050 / wheel-encoder / VL53L0X / motor-driver code once the
chassis is built, without changing any FSM logic.

## Files

| File | Status | Description |
|---|---|---|
| `graph.h` | Placeholder data, real logic | Warehouse topological graph (nodes + edge weights). Update with surveyed distances once the physical layout is fixed. |
| `A*.h` | Complete | Shortest-path planning and edge-blocking for replanning. |
| `state_machine.h` | Complete | Navigation FSM: PLANNING → MOVING → ARRIVING (RFID check) → OBSTACLE_WAIT/REPLANNING → TASK_COMPLETE, with a LOST recovery state. |
| `sensors_stub.h` | Stubbed, TODOs marked | Placeholder odometry, RFID, ToF, and motor functions. To be replaced with real sensor code. |
| `TopoBin_AMR_firmware.ino` | Complete (simulation) | Entry point; runs one simulated task end-to-end via Serial output. |

## How navigation works

1. **Planning:** A* computes the shortest node-to-node path on the topological graph.
2. **Edge traversal:** wheel-encoder + IMU dead-reckoning estimates progress along the current edge.
3. **Node confirmation:** an RFID tag read at each node resets the accumulated odometry drift to zero — this is why the system doesn't need continuous (SLAM-style) localization, only periodic ground-truth checkpoints.
4. **Obstacle handling:** if a ToF-detected obstacle persists past the wait window, the current edge is marked blocked and A* re-runs from the robot's current node.



## Next steps (post-shortlisting)

- Replace `sensors_stub.h` functions with real driver code
- Replace `graph.h` placeholder distances with surveyed warehouse measurements
- Add the web UI task-assignment layer
- Add battery-voltage monitoring and low-battery return-to-home logic
