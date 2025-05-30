#ifndef temp_H
#define temp_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

class temp{
public: 
	float tempF;
	temp();
	unsigned int getTemp();
	
private:
	DIR *dir;
	struct dirent *dirent;
	
	char dev[16];      // Dev ID
	char devPath[128]; // Path to device
	char buf[256];     // Data from device
	char tmpData[6];   // Temp C * 1000 reported by device 
	char path[]; 
	ssize_t numRead;
};
#endif
