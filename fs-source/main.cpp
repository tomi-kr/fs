#include <windows.h>
#include <string>

import file_system;

int main() {
	
	build_package_file("d:\\input\\", "d:\\content.package");

	report("\n\nDone!\n");

	while (1) {
		Sleep(100);
	}
}

