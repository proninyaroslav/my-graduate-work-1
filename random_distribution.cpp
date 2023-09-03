#include <iostream>
#include <string>
#include <map>
#include <random>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;

#define MAX_SPEED 60

int get_rand()
{
	float r = (float)rand() / (RAND_MAX + 1.0) * MAX_SPEED;

	return round(log(1 + r) * 14.71);
}

int main()
{
	srand(time(NULL));

	map<int, int> hist;
	int n = 100000;

	for(int i = 0; i < n; i++)
		++hist[get_rand()];

	for(map<int, int>::iterator it = hist.begin(); it != hist.end(); it++) {
		cout << it->first << " ";
		for (int i = 0; i < it->second / 100; i++)
			cout << "*";
		cout << endl;
	}
	int sum = hist[5] + hist[10] + hist[20] + hist[60];
	cout << endl;
	cout << 5 << " " << (double)hist[5] / sum << endl;
	cout << 10 << " " << (double)hist[10] / sum << endl;
	cout << 20 << " " << (double)hist[20] / sum << endl;
	cout << 60 << " " << (double)hist[60] / sum << endl;
}
