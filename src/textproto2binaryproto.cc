#include <string>
#include <iostream>
#include <vector>

#include <google/protobuf/text_format.h>

#include "fildesh.h"
#include "protobuf_schemae.hh"

using google::protobuf::Message;
using google::protobuf::TextFormat;


int main(int argc, char** argv) {
  const unsigned descriptor_index_offset = 4;
  if (argc <= static_cast<int>(descriptor_index_offset)) {
    std::cerr
      << "Please provide an input, output, message name, and descriptor set file(s)."
      << std::endl;
    return 64;
  }

  const std::string message_name = argv[3];

  std::unique_ptr<ProtobufSchemae> schemae;
  schemae = ProtobufSchemae::from_descriptor_files(
      std::vector<std::string>(&argv[descriptor_index_offset], &argv[argc]));
  if (!schemae) {
    return 65;
  }

  std::unique_ptr<Message> message = schemae->new_message(message_name);
  if (!message) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return 65;
  }

  // Read.
  FildeshX* in = open_FildeshXF(argv[1]);
  if (!in) {
    std::cerr << "Cannot open input file: " << argv[1] << std::endl;
    return 65;
  }
  slurp_FildeshX(in);
  const std::string in_content(in->at, in->size);
  close_FildeshX(in);

  // Parse.
  if (!TextFormat::ParseFromString(in_content, message.get())) {
    std::cerr << "Error parsing textproto." << std::endl;
    return 65;
  }

  // Encode & Write.
  if (0 == schemae->write_message_to_binary_file(argv[2], *message)) {
    return 74;
  }

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
