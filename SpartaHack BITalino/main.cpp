
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
//BITalino::Frame f;

bool updateTrigger = false;
BITalino::Vbool outputs = { 0,0,0,0 };

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

/*

void checkFaceplant(BITalino & bit, int avg) {
	BITalino::VFrame checker(100);
	bool done = false;
	while (!done)  {
		bit.read(checker);
		time_t start = clock();
		while (abs(avg - checker[0].analog[PRESSURE_SENSOR]) > 100) {
			bit.read(checker);
			if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > 2) {
				//std::thread t1(flashOutputs, bit); //flashOutputs(bit)
				//t1.join();
				flashOutputs(bit);
				done = true;
				//return;
			}
		}
	}
}

void checkFaceplant2(BITalino & bit) {
	bool done = false;
	while (!done) {
		int recentReading = f.analog[PRESSURE_SENSOR];
		time_t start = clock();
		while (abs(touchSensorAverage - recentReading) > 100) {
			recentReading = f.analog[PRESSURE_SENSOR];
			if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > 2) {
				//std::thread t1(flashOutputs, bit); //flashOutputs(bit)
				//t1.join();
				flashOutputs2(bit);
				done = true;
				//break;?
				//return;
			}
		}
	}
}

*/

void checkFaceplant3(BITalino & bit) {
	bool done = false;
	while (!done) {
		int recentReading = f->analog[PRESSURE_SENSOR];
		time_t start = clock();
		while (abs(touchSensorAverage - recentReading) > 100) {
			recentReading = f->analog[PRESSURE_SENSOR];
			if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC > 2) {
				//std::thread t1(flashOutputs, bit); //flashOutputs(bit)
				//t1.join();
				flashOutputs2(bit);
				done = true;
				//break;//?
				return;
			}
		}
	}
}


void interfacer(BITalino & bit) { //For bridging thread. Thread updates sensor data, senses certain variables to trigger output
	

	//FRAMES DEFAULTED TO 100!!! Check documentation """"If the vector is empty, it is resized to 100 frames.""""

	for (int i = 0; i < initLength; i++) {
		bit.read(frames); 
		touchSensorAverage += frames[0].analog[PRESSURE_SENSOR];
		printf("%d\n", frames[0].analog[PRESSURE_SENSOR]);
		wait(100);
	}
	touchSensorAverage = touchSensorAverage / initLength;
	printf("\n %d", touchSensorAverage);

	while (true) {
		bit.read(frames);
		f = &frames[0];

		printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n",  
			f->seq,
			f->digital[0], f->digital[1], f->digital[2], f->digital[3],
			f->analog[0], f->analog[1], f->analog[2], f->analog[3], f->analog[4], f->analog[5]);
		
		if (updateTrigger) {
			bit.trigger(outputs); //PROBLEM LINE, NOT TO BE CALLED EVERY TIME
			//resize frames to size 10
			updateTrigger = false;
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

      dev.battery(10);  // set battery threshold (optional)
      dev.start(1000, {0, 1, 2, 3, 4, 5});   // start acquisition of all channels at 1000 Hz
	  // END OF PRE-INITIALIZATION

	  /*
	  int initLength = 10;
	  int touchSensorAverage = 0;
	  for (int i = 0; i < initLength; i++) {
		  BITalino::VFrame frames(400);
		  dev.read(frames);
		  touchSensorAverage += frames[0].analog[PRESSURE_SENSOR];
		  printf("%d\n", frames[0].analog[PRESSURE_SENSOR]);
	  }
	  touchSensorAverage = touchSensorAverage / initLength;
	  printf("\n %d", touchSensorAverage);
	  */

	  flashOutputs(dev);

	  dev.trigger({ 1,1,1,1 });
	  std::string go; std::cin >> go;
	  dev.trigger({ 0,0,0,0 });

      //BITalino::VFrame frames(100); // initialize the frames vector with 100 frames 

	  //checkFaceplant(dev, 220);
	  //std::thread checkFaceplantThread(checkFaceplant, dev, touchSensorAverage);
	  //checkFaceplantThread.join();

	  //std::thread t1(interfacer, dev);

	  for (int i = 0; i < initLength; i++) {
		  dev.read(frames);
		  touchSensorAverage += frames[0].analog[PRESSURE_SENSOR];
		  printf("%d\n", frames[0].analog[PRESSURE_SENSOR]);
		  wait(100);
	  }
	  touchSensorAverage = touchSensorAverage / initLength;
	  printf("\n %d", touchSensorAverage);

	  do
	  {
		  dev.read(frames); // get 100 frames from device
		  /*const BITalino::Frame & */f = &frames[0];  // get a reference to the first frame of each 100 frames block
		  
		  //printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n",   // dump the first frame
			 // f.seq,
			 // f.digital[0], f.digital[1], f.digital[2], f.digital[3],
			 // f.analog[0], f.analog[1], f.analog[2], f.analog[3], f.analog[4], f.analog[5]);

		  printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n",   // dump the first frame
			  f->seq,
			  f->digital[0], f->digital[1], f->digital[2], f->digital[3],
			  f->analog[0], f->analog[1], f->analog[2], f->analog[3], f->analog[4], f->analog[5]);

		  if (updateTrigger) {
			  dev.trigger(outputs); //PROBLEM LINE, NOT TO BE CALLED EVERY TIME
			  //resize frames to size 10
			  updateTrigger = false;
		  }

	  } while (!keypressed()); // until a key is pressed

	  //t1.join();

      dev.stop(); // stop acquisition
   } // dev is destroyed here (it goes out of scope)
   catch (BITalino::Exception &e)
   {
      printf("BITalino exception: %s\n", e.getDescription());
   }
   
   return 0;
}
