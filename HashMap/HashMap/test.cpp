#include"AVLTree.h"
#include"HashMap.h"
#include<ctime>
#include<iostream>
#include<unordered_map>
#include<map>
#include<vector>
#include<string>
#define SIZE 100000
#define TEST 10000
using namespace std;
size_t Hasher1(const int& x) {
	return x / 100;
}
template <typename _Key>
void shuffle(vector<_Key>&M) {
	int size = M.size();
	while (--size > 0) {
		swap(M[size], M[rand() % size]);
	}

}

int main() {
	time_t x;
	time(&x);
	srand((unsigned int)x);
	{
		Hash_Map<int, int, decltype(Hasher1)*> testmap(Hasher1);
		unordered_map<int, int, decltype(Hasher1)*> Map(64, Hasher1);
		vector<int> TestArray;
		for (int i = 0; i < 2; i++) {
			cout << "---------------test case: " << i << "----------------" << endl;
			TestArray.clear();
			Map.clear();
			testmap.clear();
			for (int i = 0; i < SIZE; i++) {
				TestArray.push_back(i);
			}
			shuffle(TestArray);
			clock_t T = clock();
			for (int i = 0; i < SIZE; i++) {
				Map[TestArray[i]] = i;
			}
			cout << "Map insert: " << clock() - T << endl;
			T = clock();
			for (int i = 0; i < SIZE; i++) {
				testmap[TestArray[i]] = i;
			}
			cout << "HashMap insert: " << clock() - T << endl;
			for (int i = 0; i < SIZE; i++) {
				if (testmap[TestArray[i]] != Map[TestArray[i]]) {
					cerr << "Error on: " << TestArray[i] << endl;
					cerr << "should be: " << Map[TestArray[i]] << endl;
					cerr << "but is: " << testmap[TestArray[i]] << endl;
				}
			}
			T = clock();
			for (int i = 0; i < TEST; i++) {
				testmap[rand() % SIZE];
			}
			cout << "HashMap: " << clock() - T << endl;
			T = clock();
			for (int i = 0; i < TEST; i++) {
				Map[rand() % SIZE];
			}
			cout << "unordered_map: " << clock() - T << endl;
			cout << "Map bucket number: " << Map.bucket_count() << endl;
			cout << "My map: " << testmap.bucket() << endl;
		}
	}

	system("pause");
	return 0;
}