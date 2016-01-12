/*
 * Matt McFarland and Yondon Fu
 * doors.c
 * 
 * token recognition for a brain teaser solver
 */

int main(void) {

	print("We're gonna solve a weird consulting question!\n");

	int doors = 100;
	int hallway[doors] = {0}; 	// 0 is closed - all doors closed to start
								// 1 is open

	int swap;
	for (int i = 2; i < doors; i++) { 	// N passes

		swap = 0;
		while ((swap * i) < doors) {
			hallway[swap * i] = !hallway[swap * i]; // if open, closed. if closed, open.
			swap++;
		}
		 	
	}

	for (int i = 0; i < doors; i++) {
		if (hallway[i]) {
			print("OPEN");
		} else {
			print("closed");
		}
	}

	return 0;
}

