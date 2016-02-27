
#include "bitalino.h"

#include <stdio.h>

#ifdef _WIN32

#include <conio.h>

#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>

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

#define PRESSURE_SENSOR 0

int main()
{
   try
   {
	  // INITIALIZATION STUFF
      puts("Connecting to device...");
  
	  BITalino dev("98:D3:31:B2:BB:BF"); 
      puts("Connected to device. Press Enter to exit.");

      std::string ver = dev.version();    // get device version string
      printf("BITalino version: %s\n", ver.c_str());

      dev.battery(10);  // set battery threshold (optional)
      dev.start(1000, {0, 1, 2, 3, 4, 5});   // start acquisition of all channels at 1000 Hz
	  // END OF PRE-INITIALIZATION

	  int initLength = 10;
	  int touchSensorAverage = 0;
	  for (int i = 0; i < initLength; i++) {
		  BITalino::VFrame frames(1000);
		  dev.read(frames);
		  touchSensorAverage += frames[0].analog[PRESSURE_SENSOR];
		  printf("%d\n", frames[0].analog[PRESSURE_SENSOR]);
	  }
	  touchSensorAverage = touchSensorAverage / initLength;
	  printf("\n %d", touchSensorAverage);

	  dev.trigger({ 1,1,1,1 });
	  std::string go; std::cin >> go;
	  dev.trigger({ 0,0,0,0 });

      BITalino::VFrame frames(100); // initialize the frames vector with 100 frames 

      do
      {
         dev.read(frames); // get 100 frames from device
         const BITalino::Frame &f = frames[0];  // get a reference to the first frame of each 100 frames block
         printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n",   // dump the first frame
                f.seq,
                f.digital[0], f.digital[1], f.digital[2], f.digital[3],
                f.analog[0], f.analog[1], f.analog[2], f.analog[3], f.analog[4], f.analog[5]);



      } while (!keypressed()); // until a key is pressed

      dev.stop(); // stop acquisition
   } // dev is destroyed here (it goes out of scope)
   catch (BITalino::Exception &e)
   {
      printf("BITalino exception: %s\n", e.getDescription());
   }
   
   return 0;
}
