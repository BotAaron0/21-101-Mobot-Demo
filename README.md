# 24-101 First Demo

Arduino sketches for the 24-101 Introduction to Mechanical Engineering Mobot first demo. The original sketches and folder names are preserved.

## Demonstrations

| Sketch folder | Current behavior |
| --- | --- |
| `10_1_First_Demo_FirstPlace` | Variant labeled FirstPlace: drives at PWM 255, counts IR line detections toward 7, then uses ultrasonic distance feedback with a 13.5 cm setpoint. |
| `9_30_First_Demo_no_autocorrection_255speed` | Earlier line-counting variant: drives at PWM 135 despite the folder name, targets 7 detections, backs up briefly, pauses, then uses distance feedback with a 15.5 cm setpoint. Computes correction values but does not apply them to the initial forward command. |
| `9_30_First_Demo_with_autocorrection_255speed_20260930211621` | Despite its historical name, the current sketch backs up until an ultrasonic reading exceeds 43 cm, performs a timed parking pivot, waits 99,999 ms, then scans wall distance and orientation. Its active loop does not count lines. |

## Open and upload

1. Open the `.ino` file inside the desired sketch folder in Arduino IDE.
2. Use this repository as the Arduino sketchbook location (restart the IDE after changing it), or copy the bundled `libraries/NewPing` and `libraries/Servo` folders to your existing sketchbook's `libraries` folder.
3. Select the board matching your Elegoo robot controller and its serial port, then verify and upload one sketch at a time. The exact board model is not recorded in the source.
4. Open Serial Monitor at **9600 baud** for diagnostics.

The bundled dependencies are **NewPing 1.9.7** and **Servo 1.3.0**. Their source, examples, notices, and available license files are retained.

## Hardware pin mapping

| Function | Pin |
| --- | --- |
| Left / right motor PWM | 5 / 6 |
| Left / right motor direction | 8 / 7 |
| Motor enable | 3 |
| Servo | 10 |
| Ultrasonic trigger / echo | 13 / 12 |
| Right / middle / left IR sensor | A0 / A1 / A2 |

## Status and calibration

These are archived demo snapshots, preserved without behavior changes. Several comments and unused constants reflect earlier experiments. Line counting happens inside a while loop and can count a single wide line multiple times. Motor feedback commands are not clamped to the PWM range, and the FirstPlace sketch contains a servo command of 240. Review calibration, timing, and physical travel before running the robot.

The repository packaging was checked against the original files. Compilation and hardware operation have not been verified in this environment.

## Attribution

The sketches retain their original 24-101 starter-code attribution to the course lab instructor. No new license is assigned to the course or student code; bundled third-party libraries retain their own notices.
