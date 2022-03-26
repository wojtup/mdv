#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mdv.hpp"

#include <vector>

#define INSERTS 1000000
#define ERASES 100000

int main(void)
{
	MDV<int> marcinkowe;
	marcinkowe.insert(0, 0);
	std::vector<int> stare = {0};

	/* Vector inserts */
	srand(213769);
	clock_t start = clock(), end;
	for (int i = 0; i < INSERTS; i++) {
		int idx = rand() % stare.size();
		stare.insert(stare.begin() + idx, rand());
	}
	end = clock();
	printf("Vector inserts took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);

	/* MDV inserts */
	srand(213769);
	start = clock();
	for (int i = 0; i < INSERTS; i++) {
		int idx = rand() % marcinkowe.length();
		marcinkowe.insert(idx, rand());
	}
	end = clock();
	printf("MDV inserts took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);

	/* Compare inserts */
	start = clock();
	bool ok = true;
	for (int i = 0; i < marcinkowe.length(); i++) {
		if (marcinkowe[i] != stare[i]) {
			printf("i: %d   mdv[i] = %d vec[i] = %d\n", i, marcinkowe[i], stare[i]);
			ok = false;
		}
	}
	end = clock();
	printf("Comparing took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);
	printf("ok? %s\n", ok ? "TRUE" : "FALSE");

	/* Vector erases */
	srand(213769);
	start = clock();
	for (int i = 0; i < ERASES; i++) {
		int idx = rand() % stare.size();
		stare.erase(stare.begin() + idx);
	}
	end = clock();
	printf("Vector erases took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);

	/* MDV erases */
	srand(213769);
	start = clock();
	for (int i = 0; i < ERASES; i++) {
		int idx = rand() % marcinkowe.length();
		marcinkowe.erase(idx);
	}
	end = clock();
	printf("MDV erases took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);

	/* Compare erases */
	start = clock();
	ok = true;
	for (int i = 0; i < marcinkowe.length(); i++) {
		if (marcinkowe[i] != stare[i]) {
			printf("i: %d   mdv[i] = %d vec[i] = %d\n", i, marcinkowe[i], stare[i]);
			ok = false;
		}
	}
	end = clock();
	printf("Comparing took: %f seconds\n", double(end - start) / CLOCKS_PER_SEC);
	printf("ok? %s\n", ok ? "TRUE" : "FALSE");

	return 0;
}
