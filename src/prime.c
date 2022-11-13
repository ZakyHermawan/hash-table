#include "prime.h"

/// <summary>
/// check if x is prime
/// </summary>
/// <param name="x">number to be checked</param>
/// <returns></returns>
int is_prime(const int x) {
	if (x < 2) {
		return 0;
	}
	int i = 2;
	while (i * i <= x) {
		if (x % i == 0) {
			return 0;
		}
		++i;
	}
	return 1;
}

/// <summary>
/// get next prime of x
/// </summary>
/// <param name="x">the number to look for the next prime number</param>
/// <returns>next time</returns>
int next_prime(int x) {
	while (!is_prime(x)) {
		++x;
	}
	return x;
}
