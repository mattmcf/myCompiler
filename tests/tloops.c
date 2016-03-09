/*
 * nested loops demonstration
 */

int main() {

	int i = 10, j;
	for(i = 0; i < 50 && i != 10; ++i) {
		int b;
		i = i + i;
		if (i != j) {
			return i;
		} else {
			return i + j;
		}
			
	}

	j = 1;
	while (j < 100 || i < 100) {
		j = j + 1;
		if (i == j)
			return j;
	}
	return 0;
}