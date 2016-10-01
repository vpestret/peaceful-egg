#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <memory>
#include <sstream>
#include "codegen.h"
#include "mapgen.h"

int main(int argc, char* argv[]) {

  std::vector<std::shared_ptr<Vertex> > gcode = generate_code(0, 6);

  std::cout << "Generated code : ";
  for (auto vrx : gcode) {
    std::cout << (std::string)(*vrx) << " ";
  }
  std::cout << std::endl;

  intersect_code_spheres(gcode);

  std::cout << "Close spheres\n";
  for (auto vrx : gcode) {
    std::cout << vrx->string_w_sp1() << "\n";
  }
  std::cout << std::endl;

  auto maplayers = generate_map_from_code(gcode);

  std::getchar();

  return 0;
}
