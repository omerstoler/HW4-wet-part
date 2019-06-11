#include <cstdlib>
#include <iostream>
#include <unistd.h>

//using namespace std;
int main()
{
  int* a0 = (int*)malloc(0);
  int* a1 = (int*)malloc(5*sizeof(int));
  std::cout << "a0 = " << a0 << std::endl;
  std::cout << "a1 = " << a1 << std::endl;
  // int* a = (void*)sbrk(0);
  // int* b = (int*) malloc(5*sizeof(int));
  // assert(a+5*sizeof(int) == b);
  //cout << "After:"<< sbrk(0);
  return 0;
}
