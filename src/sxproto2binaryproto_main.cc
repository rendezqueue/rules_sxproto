#include <iostream>

#include "protobuf_schemae.hh"
#include "protobuf_transcode.hh"


int main(int argc, char** argv) {
  const unsigned descriptor_index_offset = 4;
  if (argc <= static_cast<int>(descriptor_index_offset)) {
    std::cerr
      << "Please provide an input, output, message name, and descriptor set file(s)."
      << std::endl;
    return 64;
  }

  std::unique_ptr<ProtobufSchemae> schemae;
  std::unique_ptr<google::protobuf::Message> message;

  schemae = ProtobufSchemae::from_descriptor_files(
      std::vector<std::string>(&argv[descriptor_index_offset], &argv[argc]));
  if (schemae) {
    message = new_message_from_sxproto_file(argv[1], argv[3], *schemae);
  }

  int exstatus = 65;
  if (message) {
    exstatus = 74;
    if (0 < schemae->write_message_to_binary_file(argv[2], *message)) {
      exstatus = 0;
    }
  }

  google::protobuf::ShutdownProtobufLibrary();
  return exstatus;
}
