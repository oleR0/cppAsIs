#include <iostream>
#include <stdlib.h>

int main(int /*argc*/, char** /*argv[]*/) {
  std::cout << "Hello World!";
  std::cout.flush();

  return std::cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
} 
