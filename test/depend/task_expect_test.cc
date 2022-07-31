#include "test/depend/task.pb.h"
#include <cassert>
#include <fstream>

int main(int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  assert(argc == 2);
  std::fstream input(argv[1], std::ios::in | std::ios::binary);

  sxproto::test::depend::TaskList msg;
  msg.ParseFromIstream(&input);

  assert(msg.shopping_list().grocery_list().items_size() == 1);
  assert(msg.shopping_list().grocery_list().items(0).name() == "beer");
  assert(msg.shopping_list().grocery_list().items(0).amount() == 99);

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
