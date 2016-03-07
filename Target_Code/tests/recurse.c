int x;

int main(void) {
	print "Recursing!";

	x = 10;
	recurse(10);

	return 0;
}

void recurse(int x) {

	if (x > 0){
		print x--;
		recurse(x);
	}

	return;
}
