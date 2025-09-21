# AYUH-IOT
IoT-based automation solution to manage and optimize the environmental conditions for hydroponic gardening. This system eliminates manual intervention by automatically controlling grow lights and water pumps based on customizable schedules, while providing remote monitoring and control capabilities through a mobile application.

# Core Problem Solved
Traditional hydroponic systems require constant manual monitoring and adjustment of light cycles and nutrient delivery. This project addresses:
Inconsistent light schedules affecting plant growth
Manual pump operation leading to over/under-watering
Lack of remote monitoring capabilities
Energy inefficiency from non-optimized equipment usage

# Key Features
1. Intelligent Light Management
Programmable ON/OFF schedules with precise time control
Manual override capability for special requirements
Support for different photoperiods based on plant types

2. Automated Nutrient Delivery
Configurable pump operation intervals (1-3 hours)
Adjustable run duration (1-60 minutes)
Time-restricted operation windows
Manual control option when needed

3. Remote Monitoring & Control
Real-time status updates via mobile app
Schedule adjustments from anywhere
System status and health monitoring
Alert notifications for anomalies

4. Smart Automation
NTP time synchronization for accurate scheduling
Power failure recovery with schedule persistence
Network outage resilience (continues local operation)
Edge case handling for various scenarios

Technical Architecture
Hardware Components
Controller: ESP8266/ESP32 microcontroller

Power Management: 5V PSU with surge protection

Switching: 2-Channel Solid State Relays (SSRs)

Loads:
200W LED Grow Lights
60W Water Pump

Connectivity: WiFi 802.11 b/g/n

Software Stack
Firmware: Arduino C++ with Blynk integration

Cloud Service: Blynk IoT platform

Mobile App: Cross-platform Blynk application

Time Service: NTP synchronization
