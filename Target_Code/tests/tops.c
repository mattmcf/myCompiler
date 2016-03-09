int x, y, z = 0;

int main() {
	int result = 0;

	int a = 15, b = 5, c = -1;

	c = a + b;
	if (c != 20) {
		print "addition failure";
		result++;
	}

	y = a + b;

	c = a - b;
	if (c != 10) {
		print "subtraction failure"; 
		result++;
	}

	c = a / b;
	if (c != 3) {
		print "division failure";
		result++;
	}

	c = (a + 2) / b;
	if (c != 3) {
		print "division failure";
		result++;
	}

	c = a * b;
	if (c != 75) {
		print "multiplication failure";
		result++;
	}
	
	c = a % b;
	if (c != 0) {
		print "mod1 failure";
		result++;
	}

	c = (a + 4) % b;
	if (c != 4) {
		print "mod2 failure";
		result++;
	}

	c = !a;
	if (c != 0) {
		print "negation1 failure";
		print !a;
		result++;
	}

	c = !0;
	if (c != 1) {
		print "negation2 failure";
		print !0;
		result++;
	}

	c = a && b;
	if (c != 1) {
		print "and1 failure";
		result++;
	}

	c = 0 && a;
	if (c != 0) {
		print "and2 failure";
		result++;
	}

	c = a && 0;
	if (c != 0) {
		print "and3 failure";
		result++;
	}

	c = 0 && 0;
	if (c != 0) {
		print "and4 failure";
		result++;
	}	

	c = a || b;
	if (c != 1) {
		print "or1 failure";
		result++;
	}

	c = 0 || a;
	if (c != 1) {
		print "or2 failure";
		result++;
	}

	c = a || 1;
	if (c != 1) {
		print "or3 failure";
		result++;
	}

	c = 0 || 0;
	if (c != 0) {
		print "or4 failure";
		result++;
	}

	c = ++a;
	if (c != 16 || a != 16) {
		print "preincrement failure";
		result++;
	}

	c = --a;
	if (c != 15 || a != 15) {
		print "predecrement failure";
		result++;
	}

	c = a--;
	if (c != 15 || a != 14) {
		print "postdecrement failure";
		result++;
	}	

	c = a++;
	if (c != 14 || a != 15) {
		print "postincrement failure";
		result++;
	}		

	c = a == b;
	if (c != 0) {
		print "equality test1 failure";
		result++;
	}

	c = a == a;
	if (c != 1) {
		print "equality test2 failure";
		result++;
	}

	c = a < a-1;
	if (c != 0) {
		print "less than1 failure";
		result++;
	}

	c = a < a;
	if (c != 0) {
		print "less than2 failure";
		result++;
	}	

	c = a < a+1;
	if (c != 1) {
		print "less than3 failure";
		result++;
	}

	c = a <= a-1;
	if (c != 0) {
		print "less than or equal to1 failure";
		result++;
	}

	c = a <= a;
	if (c != 1) {
		print "less than or equal to2 failure";
		result++;
	}	

	c = a <= a+1;
	if (c != 1) {
		print "less than or equal to3 failure";
		result++;
	}

	c = a > a-1;
	if (c != 1) {
		print "greater than1 failure";
		result++;
	}

	c = a > a;
	if (c != 0) {
		print "greater than2 failure";
		result++;
	}	

	c = a > a+1;
	if (c != 0) {
		print "greater than3 failure";
		result++;
	}	

	c = a >= a-1;
	if (c != 1) {
		print "greater than or equal to1 failure";
		result++;
	}

	c = a >= a;
	if (c != 1) {
		print "greater than or equal to2 failure";
		result++;
	}	

	c = a >= a+1;
	if (c != 0) {
		print "greater than or equal to3 failure";
		result++;
	}

	c = a != b;
	if (c != 1) {
		print "not equal1 failure";
		result++;
	}

	c = a != a;
	if (c != 0) {
		print "not equal2 failure";
		result++;
	}

	print "IO testing";
	print a;
	print "hello";
	print "Enter some input!";
	read b;

	if (result > 0) {
		print "tests failed!";
	} else {
		print "All tests passed!";
	}

	return 0;
}