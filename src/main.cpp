#include <iostream>
#include <vector>

#include "async.h"

using namespace std;

int main(int argc, const char *argv[]) {
  std::size_t bulk = 5;
  auto h = async::connect(bulk);
  auto h2 = async::connect(bulk);
  async::receive(h, "1", 1);
  async::receive(h2, "1\n", 2);
  async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
  async::receive(h, "b\nc\nd\n}\n89\n", 11);
  async::disconnect(h);
  async::disconnect(h2);

  auto h3 = async::connect(bulk);
  async::receive(h3, "55", 2);
  async::disconnect(h3);

  async::finalize();

  return 0;
}
