
#include "bitalino.h"
#include <stdio.h>

#ifdef _WIN32

#include <conio.h>

#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <thread>

bool keypressed(void)
{
	return (_kbhit() != 0);
}

#else // Linux or Mac OS

#include <sys/select.h>

bool keypressed(void)
{
   fd_set   readfds;
   FD_ZERO(&readfds);
   FD_SET(0, &readfds);

   timeval  readtimeout;
   readtimeout.tv_sec = 0;
   readtimeout.tv_usec = 0;

   return (select(FD_SETSIZE, &readfds, NULL, NULL, &readtimeout) == 1);
}

#endif

#define PRESSURE_SENSOR 3

int initLength = 10;
int touchSensorAverage = 0;
BITalino::VFrame frames(10); //DEFAULTED TO 100!!! Check documentation
BITalino::Frame* f;// = &frames[0];
BITalino::Frame g;

bool updateTrigger = false;
BITalino::Vbool outputs = { 0,0,0,0 };

int sleepCount = 0;

void wait(unsigned int waitTime) {
	time_t now = clock();
	while (clock() - now < waitTime);
}

void flashOutputs(BITalino & bit) {
	for (int i = 0; i < 6; i++) {
		bit.trigger({ 1,1,1,1 });
		wait(50);
		bit.trigger({ 0,0,0,0 });
		wait(50);
	}
}

void flashOutputs2(BITalino & bit) {
	BITalino::Vbool temp = outputs;

	for (int i = 0; i < 6; i++) {
		outputs = { 1,1,1,1 };
		updateTrigger = true;
		wait(50);
		//updateTrigger = false;
		outputs = { 0,0,0,0 };
		updateTrigger = true;
		wait(50);
		//updateTrigger = false;
	}

	outputs = temp;
}

void checkFaceplant4(BITalino & bit) { 
	while (true) {
		int recentReading = g.analog[PRESSURE_SENSOR];
		time_t start = clock();
		while (abs(touchSensorAverage - recentReading) > 100) { //trigger a thread?
			recentReading = g.analog[PRESSURE_SENSOR];
			if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > 4) {
				//user has fallen asleep 
				
				//std::thread t1(flashOutputs, bit); //flashOutputs(bit)
				//t1.join();
				flashOutputs(bit); //on a thread?
				//done = true;

				sleepCount++;
				wait(500); //pause not visable in consol because separate thread
				printf("%d \n", sleepCount);
				printf("%d \n", sleepCount);
				printf("%d \n", sleepCount);
				wait(500); //pause not visable in consol because separate thread		
			}
		}
	}
}

int main() {
   try {
	  // INITIALIZATION STUFF
      puts("Connecting to device...");
  
	  BITalino dev("98:D3:31:B2:BB:BF"); 
      puts("Connected to device. Press Enter to exit.");

      std::string ver = dev.version();    // get device version string
      printf("BITalino version: %s\n", ver.c_str());

	  printf("Maximum threads: %d\n", std::thread::hardware_concurrency());

      dev.battery(10);  // set battery threshold (optional)
      dev.start(1000, {0, 1, 2, 3, 4, 5});   // start acquisition of all channels at 1000 Hz
	  // END OF PRE-INITIALIZATION

	  flashOutputs(dev);

	  dev.trigger({ 1,1,1,1 });
	  std::string go; std::cin >> go;
	  dev.trigger({ 0,0,0,0 });

	  for (int i = 0; i < initLength; i++) {
		  dev.read(frames);
		  touchSensorAverage += frames[0].analog[PRESSURE_SENSOR];
		  printf("%d\n", frames[0].analog[PRESSURE_SENSOR]);
		  wait(100);
	  }
	  touchSensorAverage = touchSensorAverage / initLength;
	  printf("\n %d", touchSensorAverage);

	  std::thread t1(checkFaceplant4, dev);
	  
	  do {
		  dev.read(frames); 
		  f = &frames[0]; 
		  g = frames[0]; 
		  
		  printf(/*%d : */ "%d %d %d %d ; %d %d %d %d %d %d\n",   
			  //f->seq,
			  g.digital[0], g.digital[1], g.digital[2], g.digital[3],
			  g.analog[0], g.analog[1], g.analog[2], g.analog[3], g.analog[4], g.analog[5]);

		  if (updateTrigger) {
			  dev.trigger(outputs); //PROBLEM LINE, NOT TO BE CALLED EVERY TIME
			  //resize frames to size 10
			  updateTrigger = false;
		  }

	  } while (!keypressed()); // until a key is pressed

	  t1.join();

      dev.stop(); // stop acquisition
   } catch (BITalino::Exception &e) {
      printf("BITalino exception: %s\n", e.getDescription());
   }
   
   return 0;
}