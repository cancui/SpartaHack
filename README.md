# SpartaHack
SpartaHack project 

by Can Cui, Nikhil Keswani, Rohan Agarwal

##Project Description
We built a headband to help you stay awake when you're studying for finals (you can guess what inspired this project...). The headband actively monitors for faceplanting and reduction of consciousness (how this is achieved will be discussed). 

The system is composed of a wireless data logger chip (Bitalino), sensors (accelerometer, EDA, touch), a circuit to collect and filter sensor data, and computer program to receive and analyse incoming sensor data. 

The computer program is coded in C++. It uses Windows threads to run distinct monitoring algorithms on different data and the Bitalino API to pull sensor data. 

##How it works
Bluetooth communication is established with the data logger from the computer program. The data logger is set to collect data from all sensors and at 1000Hz and send the data via Bluetooth (It's preloaded with software for sampling, bluetooth communication, and executing simple IO commands). We implemented analog filtering in the hardware to reduce the noise in sensor data. 

The computer program runs several threads. The main thread is used to pull sensor data from the data logger. Then, each sensor is allocated its own thread, in order to run the algorithm to analyse data of that particular sensor. This way, we can concurrently process sensor data of many types. 

The current implementation runs one thread for touch sensor and one thread for EDA. Theoretically, we could have as many sensors as the computer can have threads (usually 4). The touch sensor thread monitors for an extended period of high analog signal (indicating touching) and the EDA thread monitors for the start of a gradual increase in analog signal (indicating increasing skin conductivity, a sign of reduction of consciousness). 

When either thread determines that the user is falling asleep, a waking function is triggered. This sends a command the data logger to make loud buzzing noises. There is circuitry implemented to amplify the weak data logger signal into a stronger one used to actuate the buzzer. The computer software tracks the number of times the user has fallen asleep. The greater this number, the more annoying the next buzz will be. 


##Future Improvement
In the future, we would like to leverage the accelerometer which we had already implemented at the hardware level in order to monitor for other signs of falling asleep, like head swaying. This would be done using a combination of acceleration and displacement data. In fact, the code to calculate displacement from analog accelerometer signals is already written as well. The challenge would be to develop an algorithm to analyse the data for features indicating the user is falling asleep. A machine learning grouping algorithm may have application here. 
