#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <ctime>
#include <algorithm>


void GenerateRandomArray(double*& arr, int size, int max)
{
	for (int i = 0; i < size; i++)
	{
		arr[i] = rand() % max;
	}
	std::cout << "Generated\n";
}

void ProcessInitialisation(double*& arr, int*& result, int& Size, double*& sortedResult) {

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

void CalcRanks(double* arr, int Size, int* pRanks)
{
	for (int i = 0; i < Size; i++)
	{
		pRanks[i] = 0;
	}


	for (int i = 0; i < Size; i++) {
#pragma omp parallel for
		for (int k = 0; k < Size; k++)
		{
			if (arr[k] > arr[i] || (arr[k] == arr[i] && k > i))
				pRanks[k]++;
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
	ProcessInitialisation(array, resultRanks, size, sortedResult);

	std::clock_t start;
	double duration;

	start = std::clock();
	CalcRanks(array, size, resultRanks);
	for (int i = 0; i < size; i++)
	{
		sortedResult[resultRanks[i]] = array[i];
	}
	duration = (std::clock() - start) / (double)((clock_t(1000)));

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
