#include <string>
#include <iostream>
#include <vector>

#include <google/protobuf/util/json_util.h>

#include "fildesh.h"
#include "protobuf_schemae.hh"

using google::protobuf::Message;
using google::protobuf::util::MessageToJsonString;


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
  if (!message->ParseFromString(in_content)) {
    std::cerr << message->DebugString() << std::endl;
    return 65;
  }


  // Encode.
  std::string out_content;
  google::protobuf::util::Status status = MessageToJsonString(
      *message, &out_content);
  if (!status.ok()) {
    std::cerr << "Error encoding json." << std::endl;
    return 74;
  }

  // Write.
  FildeshO* out = open_FildeshOF(argv[2]);
  if (!out) {
    std::cerr << "Error opening output file: " << argv[2] << std::endl;
    return 73;
  }
  put_bytestring_FildeshO(
      out,
      (const unsigned char*)out_content.data(),
      out_content.size());
  close_FildeshO(out);

  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
