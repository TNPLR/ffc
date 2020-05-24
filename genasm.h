#ifndef GENASM_H_
#define GENASM_H_
#include "topc.h"

#include <sstream>

namespace GenAsm {
std::stringstream& generate(Topc &topc);
}
#endif /* GENASM_H_ */
