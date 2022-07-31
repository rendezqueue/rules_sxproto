#include "test/depend/shopping.pb.h"
#include <cassert>
#include <fstream>

int main(int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  assert(argc == 2);
  std::fstream input(argv[1], std::ios::in | std::ios::binary);

  sxproto::test::depend::ShoppingList msg;
  msg.ParseFromIstream(&input);

  assert(msg.grocery_list().items_size() == 2);
  assert(msg.grocery_list().items(0).name() == "jar of olives");
  assert(msg.grocery_list().items(0).amount() == 1);
  assert(msg.grocery_list().items(1).name() == "tortilla");
  assert(msg.grocery_list().items(1).amount() == 20);

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
