/*
 Name: Daibik DasGupta
 RowanID: 916479074
 Homework #: 2
 To Compile: gcc -o ouput -fopenmp assignment2.c
 To Run: ./output <rows> <columns> <max_generations> <num_threads>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

double gettime(void)
{
	struct timeval tval;
  gettimeofday(&tval, NULL);
  return((double)tval.tv_sec + (double)tval.tv_usec/1000000.0);
}

int **array_alloc(int **a, int rm, int cm)
{
	//storing 2D array as an array of pointers
	a = (int**) malloc((rm+2) * sizeof(int));
	for(int i=0; i<rm+2; i++)
		a[i] = (int*) malloc((cm+2) * sizeof(int));
	return a;
}

void array_print(int **a, int rm, int cm)
{
	for(int i=1; i<rm+1; i++)
	{
		printf("\n|");
		for(int j=1; j<cm+1; j++)
		{
			if(a[i][j] == 0)
				printf("   |");
			else
				printf(" * |");
		}
	}
}

int **array_ghostcells(int **a, int rm, int cm)
{
	for(int j=1; j<cm+1; j++)
	{
		a[0][j] = a[rm][j];
		a[rm+1][j] = a[1][j];
	}
	for(int i=1; i<rm+1; i++)
	{
		a[i][0] = a[i][cm];
		a[i][cm+1] = a[i][1];
	}
	//copying diagonal elements manually
	a[0][0]=a[rm][cm];
	a[0][cm+1]=a[rm][1];
	a[rm+1][0]=a[1][cm];
	a[rm+1][cm+1]=a[1][1];
	return a;
}

void array_copy(int **a, int **b, int rm, int cm)
{
	for(int i=1; i<rm+1; i++)
	{
		for(int j=1; j<cm+1; j++)
			a[i][j] = b[i][j];
	}
}

int array_aresame(int **a, int **b, int rm, int cm)
{
	int flag = 1;
	//if even a single value does not match, the flag will be lowered
	for(int i=1; i<rm+1; i++)
	{
		for(int j=1; j<cm+1; j++)
			if(a[i][j] != b[i][j])
				flag = 0;
	}
	return flag;
}

int count_living_cells(int **a, int r, int c)
{
	int count=0;
	//traverse from top-left to bottom-right element to check neighbors
	for(int i=r-1; i<=r+1; i++)
	{
		for(int j=c-1; j<=c+1; j++)
		{
			if((i==r) && (j==c))
				continue;
			if(a[i][j]==1)
				count++;
		}
	}
	return count;
}

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
  	printf("Usage: %s <rows> <columns> <max_generations> <num_threads>\n", argv[0]);
  	return 1;
  }

  int row = atoi(argv[1]);
  int col = atoi(argv[2]);
  int generations = atoi(argv[3]);
	int nthreads = atoi(argv[4]);
  int cellcount, gencount = 1;
  int **aptr = NULL, **bptr = NULL;
  double starttime, endtime;

	//dynamically allocating two arrays
  aptr = array_alloc(aptr, row, col);
  bptr = array_alloc(bptr, row, col);

	//using the first array to represent initial state
	for(int i=1; i<row+1; i++)
		for(int j=1; j<col+1; j++)
			aptr[i][j] = rand()%2;

	//generating ghost cells in the array
  aptr = array_ghostcells(aptr, row, col);

#ifdef PRINT_GEN
	printf("Initial Status:");
	array_print(aptr, row, col);
#endif

  starttime = gettime();

  while (gencount <= generations)
	{
		//generate each cell of the next generation by checking the current cells
#pragma omp parallel for shared(aptr, bptr) private(cellcount) num_threads(nthreads)
		for(int i=1; i<row+1; i++)
		{
			for(int j=1; j<col+1; j++)
			{
				cellcount = count_living_cells(aptr, i, j);
				//using the rules of the game to decide the status
				if(aptr[i][j]==1)
					if(cellcount < 2 || cellcount > 3)
						bptr[i][j] = 0;
					else
						bptr[i][j] = 1;
				else
					if(cellcount == 3)
						bptr[i][j] = 1;
					else
						bptr[i][j] = 0;
			}
		}

#pragma omp barrier

#ifdef PRINT_GEN
		printf("\nGeneration %d:", gencount);
		array_print(bptr, row, col);
#endif

		//if new generation and prior generation are same, the game ends
		if(array_aresame(aptr, bptr, row, col) == 1)
			break;

		//copying current generation to aptr. next generation will be in bptr again
		array_copy(aptr, bptr, row, col);
		aptr = array_ghostcells(aptr, row, col);

#pragma omp barrier

		gencount++;
	}

	endtime = gettime();
	printf("\nExecution time for generations: %lf seconds\n", endtime-starttime);

	return 0;
}
