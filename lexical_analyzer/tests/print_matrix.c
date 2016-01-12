/*
 * Matt McFarland and Yondon Fu
 * sample.c
 * 
 * Testing token recognition for if's and comparision
 */

int size = 100;

void main(void) {

	if (size < 0) {
		print("can't print a negatively sized matrix!");
		return;
	}

	for (int i = 0; i < size; i++) { 		// row
		for (int j = 0; j < size; j++) {	// column
			if (j == i && i == size / 2) {
				print("X");
			} else if ( (j == size / 2) || (i == size / 2) ) {
				print("_");
			} else if (j == i) {
				print("#");
			} else if (j < i) {
				print("@");
			} else {
				print("&");
			}
		}
	}

	return;
}