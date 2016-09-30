#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include "codegen.h"

int main(int argc, char* argv[]) {

  std::vector<std::shared_ptr<Vertex> > gcode = generate_code(0);

  std::cout << "Generated code : ";
  for (auto vrx : gcode) {
    std::cout << (std::string)(*vrx) << " ";
  }
  std::cout << std::endl;

  std::getchar();

  return 0;
}
