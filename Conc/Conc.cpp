#include <stdio.h>
#include <iostream>
#include <ctime>

void GenerateRandomArray(double*& arr, int size, int max)
{
	for (int i = 0; i < size; i++)
	{
		arr[i] = rand() % max;
	}
	std::cout << "Generated\n";
}

void AllocateArrays(double*& arr, int*& result, int& Size, double*& sortedResult) {

	do {
		printf("\nEnter the size of the array: ");
		std::cin >> Size;
		if (Size < 0) {
			printf("Size of the array must be >= than number of processes! \n");
		}
	} while (Size < 0);

	result = new int[Size];
	sortedResult = new double[Size];

	arr = new double[Size];
		GenerateRandomArray(arr, Size, 100);
}

void CalcRanks(double* arr, int Size, int* resultRanks)
{
	for (int i = 0; i < Size; i++)
	{
		resultRanks[i] = 0;
	}

	for (int i = 0; i < Size; i++) {
		for (int k = 0; k < Size; k++)
		{
			if (arr[k] > arr[i] || (arr[k] == arr[i] && k > i))
				resultRanks[k]++;
		}
	}
}

void TerminateProcess(double*& arr, int*& resultRanks, int& Size, double*& result)
{
	delete[] arr;
	delete[] resultRanks;
	delete[] result;
}

void PrintArray(double* arr, int& size)
{
	for (int i = 0; i < size; i++)
	{
		printf("%f; ", arr[i]);
	}
}

int main(int argc, char* argv[])
{
	double* array;
	int* resultRanks;
	int size;
	double* sortedResult;
	//Initialise array
	AllocateArrays(array, resultRanks, size, sortedResult);
	std::clock_t start;
	double duration;

	start = std::clock();
	
	CalcRanks(array, size, resultRanks);

	for (int i = 0; i < size; i++)
	{
		sortedResult[resultRanks[i]] = array[i];
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	printf("Time: %f; \n", duration);

	bool pr = false;
	std::cout << "Print the array? 1 or 0: ";
	std::cin >> pr;
	if (pr)
	{
		PrintArray(sortedResult, size);
	}

	TerminateProcess(array, resultRanks, size, sortedResult);

	return 0;
}
