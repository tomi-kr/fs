#include <windows.h>
#include <string>
#include "timer.h"

import file_system;

int main() {
	
	
	double start = High_Res_Timer::msec();

	//build_package_file("d:\\input\\", "d:\\content.package");
	build_package_file("c:\\projects\\quakespasm", "d:\\content.package");


	double end = High_Res_Timer::msec();

	report("\n\nDone " + std::to_string(end - start) + "\n");

	while (1) {
		Sleep(100);
	}
}

