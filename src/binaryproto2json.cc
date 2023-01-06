#include <string>
#include <iostream>
#include <vector>

#include <google/protobuf/util/json_util.h>

#include <fildesh/fildesh.h>
#include "protobuf_schemae.hh"

using google::protobuf::Message;
using google::protobuf::util::JsonPrintOptions;
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

  // Read & Parse.
  std::unique_ptr<Message> message =
    schemae->new_message_from_binary_file(argv[1], message_name);

  // Encode.
  std::string out_content;
  JsonPrintOptions json_options;
  json_options.preserve_proto_field_names = true;
  google::protobuf::util::Status status = MessageToJsonString(
      *message, &out_content, json_options);
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
