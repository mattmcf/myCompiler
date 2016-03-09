/*
 * tests features of sizeof()
 */

int sub_call(int param, int param_arr[]) {
	int result;

	if (sizeof(param) != 4) {
		print "sizeof(a) != 4";
		result++;
	}

	if (sizeof(param_arr[0]) != 4) {
		print "sizeof(param_arr[0]) != 4";
		print sizeof(param_arr[0]);
		result++;
	}

	print "Note: cannot call sizeof(param_arr) because array size is unknown";
	return result;
}

int main() {
	int result = 0;
	int a = 10;
	int arr[10];

	print "Testing sizeof() functionality";

	if (sizeof(a) != 4) {
		print "sizeof(a) != 4";
		result++;
	}

	if (sizeof(arr) != 40) {
		print "sizeof(arr) != 40";
		result++;
	}

	if (sizeof(arr[0]) != 4) {
		print "sizeof(arr[0]) != 4";
		print sizeof(arr[0]);
		result++;
	}

	result = result + sub_call();

	if (result == 0) {
		print "all tests passed";
	} else {
		print "some sizeof() tests failed";
	}

	return result;
}