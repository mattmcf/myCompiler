int x, y, z = 0;

int main() {
	int a, b, c;

	a = 10;
	b = a;

	c = a + b;
	y = a + b;
	c = a - b;
	c = a / b;
	c = a * b;
	c = a % b;
	c = !a;
	c = a && b;
	c = a || b;
	c = ++a;
	c = --a;
	c = a--;
	c = a++;

	c = a == b;
	c = a < b;
	c = a <= b;
	c = a > b;
	c = a >= b;
	c = a != b;

	print a;
	print "hello";
	print "Enter some input!";
	read b;

	return 0;
}