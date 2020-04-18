/*************************************************************************
 * libjson-rpc-cpp-skale
 *************************************************************************
 * @file    main.cpp
 * @date    29.09.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include <iostream>
#include <string>

#include "stubgeneratorfactory.h"

using namespace std;
using namespace jsonrpcskale;

int main(int argc, char **argv) {
  vector<StubGenerator *> stubgens;
  vector<Procedure> procedures;

  bool result = StubGeneratorFactory::createStubGenerators(
      argc, argv, procedures, stubgens, stdout, stderr);

  for (unsigned int i = 0; i < stubgens.size(); ++i) {
    stubgens[i]->generateStub();
  }
  StubGeneratorFactory::deleteStubGenerators(stubgens);
  return !result;
}
