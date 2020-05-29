
#include "mpi.h"

#include <stdio.h>
#include <iostream>


int ProcRank, ProcNumber;

void GenerateRandomArray(double*& arr, int size, int max)
{
	for (int i = 0; i < size; i++)
	{
		arr[i] = rand() % max;
	}
	std::cout << "Generated\n";
}

void DistributeData(double* arr, int* procNumElems, int* procStartInd, int Size) {
	//pSendNum - the number of elements sent to the process
	//pSendInd the index of the first data element sent to the process
	int* pSendNum = new int[ProcNumber];
	int* pSendInd = new int[ProcNumber];


	int UnallocatedElems = Size;
	int UnallocatedProc = ProcNumber;

	// Determine the disposition of the matrix rows for current process

	for (int i = 0; i < ProcNumber; i++)
	{
		pSendNum[i] = UnallocatedElems / UnallocatedProc;
		pSendInd[i] = Size - UnallocatedElems;
		UnallocatedElems -= pSendNum[i];
		UnallocatedProc -= 1;
	}
	MPI_Scatter(pSendNum, 1, MPI_INT, procNumElems, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(pSendInd, 1, MPI_INT, procStartInd, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void ProcessInitialisation(double*& array, int*& resultRanks, int& size, double*& sortedResult, int*& pNumberRanks, int& pStartInd, int& pNumElems) {
	if (ProcRank == 0)
	{
		do {
			printf("\nEnter the size of the array: ");
			std::cin >> size;
			if (size < ProcNumber) {
				printf("Size of the array must be >= than number of processes! \n");
			}
		} while (size < ProcNumber);
	}
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	resultRanks = new int[size];
	sortedResult = new double[size];

	array = new double[size];

	if (ProcRank == 0) {
		GenerateRandomArray(array, size, 100);
	}

	MPI_Bcast(array, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	DistributeData(array, &pNumElems, &pStartInd, size);

	pNumberRanks = new int[pNumElems];
}



void TestDistribution(double* arr, int Size, int pNumElems, int pStartInd) {
	if (ProcRank == 0) {
		printf("Array: \n");
		for (int i = 0; i < Size; i++)
		{
			printf("%f; ", arr[i]);
		}
	}
	fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD);
	for (int i = 0; i < ProcNumber; i++)
	{
		if (ProcRank == i) {
			printf("\nProcRank = %d, Start Index = %d, Num = %d \n", ProcRank, pStartInd, pNumElems);
			for (int k = pStartInd; k < pNumElems + pStartInd; k++)
			{
				printf("%f; ", arr[k]);
			}
		}
		fflush(stdout);
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

void CalcArrayRanks(double* arr, int Size, int pNumElems, int pStartInd, int* pRanks)
{
	for (int i = 0; i < pNumElems; i++)
	{
		pRanks[i] = 0;
	}

	for (int i = 0; i < Size; i++) {
		for (int k = pStartInd; k < pNumElems+pStartInd; k++)
		{
			if (arr[k] > arr[i] || (arr[k] == arr[i] && k > i))
				pRanks[k - pStartInd]++;
		}
	}
	/*for (int i = 0; i < ProcNumber; i++)
	{
		if (ProcRank == i) {
			printf("\nSORT: ProcRank = %d, Start Index = %d, Num = %d \n", ProcRank, pStartInd, pNumElems);
			for (int k = 0; k < pNumElems; k++)
			{
				printf("%d; ", pRanks[k]);
			}
		}
		fflush(stdout);
		MPI_Barrier(MPI_COMM_WORLD);
	}*/
}

void GatherResults(int* pProcResult, int* result, int Size) {
	int* receiveNum = new int[ProcNumber];
	int* receiveInd = new int[ProcNumber];
	int RestParts = Size;
	receiveInd[0] = 0;
	receiveNum[0] = Size / ProcNumber;
	for (int i = 1; i < ProcNumber; i++) {
		RestParts -= receiveNum[i - 1];
		receiveNum[i] = RestParts / (ProcNumber - i);
		receiveInd[i] = receiveInd[i - 1] + receiveNum[i - 1];
	}

	MPI_Allgatherv(pProcResult, receiveNum[ProcRank], MPI_INT, result,
		receiveNum, receiveInd, MPI_INT, MPI_COMM_WORLD);

	delete[] receiveNum;
	delete[] receiveInd;
}

void TerminateProcess(double*& arr, int*& resultRanks, int& Size, double*& result, int*& pNumberRanks)
{
	delete[] arr;
	delete[] resultRanks;
	delete[] result;
	delete[] pNumberRanks;
}

void PrintArray(double* arr, int& size)
{
	printf("%d; ", 1);
	for (int i = 0; i < size; i++)
	{
		printf("%f; ", arr[i]);
	}
}

int main(int argc, char *argv[])
{


	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNumber);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0) 
		printf("MPI World size = %d processes\n", ProcNumber);

	double* array;
	int* resultRanks;
	int size;
	int* pNumberRanks;
	double* sortedResult;
	int pNumElems;
	int pStartInd;

	//Initialise and distribute

	///
	if (ProcRank == 0)
	{
		do {
			printf("\nEnter the size of the array: ");
			std::cin >> size;
			if (size < ProcNumber) {
				printf("Size of the array must be >= than number of processes! \n");
			}

		} while (size < ProcNumber);
	}
	
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	resultRanks = new int[size];
	sortedResult = new double[size];

	array = new double[size];

	if (ProcRank == 0) {
		GenerateRandomArray(array, size, 100);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	auto start = MPI_Wtime();

	MPI_Bcast(array, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	DistributeData(array, &pNumElems, &pStartInd, size);

	pNumberRanks = new int[pNumElems];
	///

	pNumberRanks = new int[pNumElems];



	CalcArrayRanks(array, size, pNumElems, pStartInd, pNumberRanks);

	GatherResults(pNumberRanks, resultRanks, size);

	if (ProcRank == 0)
	{
		for (int i = 0; i < size; i++)
		{
			sortedResult[resultRanks[i]] = array[i];
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	auto end = MPI_Wtime();

	if (ProcRank == 0)
	{
		printf("Time: %f \n", end - start);
		bool pr = false;
		std::cout << "Print the array? 1 or 0: ";
		std::cin >> pr;
		if (pr)
		{
			PrintArray(sortedResult, size);
		}
	}

	TerminateProcess(array, resultRanks, size, sortedResult, pNumberRanks);


	MPI_Finalize();


	return 0;
}
