#include <stdio.h>
#include "raycast.h"

void printSetupMessages() {
	printf("Starting " PROJECT_NAME "... \n");
	printf("Operating system: ");

	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
			printf("Windows ");
   		#ifdef _WIN64
			printf("64-bit\n");
   		#else
			printf("32-bit\n");
   	#endif
	#elif __APPLE__
    	#include <TargetConditionals.h>
		#if TARGET_IPHONE_SIMULATOR
			printf("Apple device simulator\n");
    	#elif TARGET_OS_MACCATALYST
			printf("Mac Catalyst\n");
    	#elif TARGET_OS_IPHONE
			printf("iOS/tvOS/watchOS\n");
    	#elif TARGET_OS_MAC
			printf("macOS\n");
    	#else
			printf("Unknown Apple platform\n");
    #endif
	#elif __ANDROID__
		printf("Android\n");
	#elif __linux__
		printf("Linux\n");
	#elif __unix__ // all unices not caught above
		printf("Unknown UNIX\n");
	#elif defined(_POSIX_VERSION)
		printf("Unknown POSIX\n");
	#else
		printf("Unknown compiler or operating system\n");
	#endif
}