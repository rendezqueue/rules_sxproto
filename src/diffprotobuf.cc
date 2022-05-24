#include <string>
#include <iostream>
#include <vector>

#include <google/protobuf/util/message_differencer.h>

#include "fildesh.h"
#include "protobuf_schemae.hh"

using google::protobuf::Message;


int main(int argc, char** argv) {
  const unsigned descriptor_index_offset = 4;
  if (argc <= static_cast<int>(descriptor_index_offset)) {
    std::cerr
      << "Please provide 2 inputs, a message name, and descriptor set file(s)."
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

  std::unique_ptr<Message> lhs_message = schemae->new_message(message_name);
  std::unique_ptr<Message> rhs_message = schemae->new_message(message_name);
  if (!lhs_message || !rhs_message) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return 65;
  }

  // Read 1.
  FildeshX* in = open_FildeshXF(argv[1]);
  if (!in) {
    std::cerr << "Cannot open input file: " << argv[1] << std::endl;
    return 65;
  }
  slurp_FildeshX(in);
  const std::string lhs_content(in->at, in->size);
  close_FildeshX(in);

  // Parse 1.
  if (!lhs_message->ParseFromString(lhs_content)) {
    std::cerr << lhs_message->DebugString() << std::endl;
    return 65;
  }

  // Read 2.
  in = open_FildeshXF(argv[2]);
  if (!in) {
    std::cerr << "Cannot open input file: " << argv[2] << std::endl;
    return 65;
  }
  slurp_FildeshX(in);
  const std::string rhs_content(in->at, in->size);
  close_FildeshX(in);

  // Parse 2.
  if (!rhs_message->ParseFromString(rhs_content)) {
    std::cerr << rhs_message->DebugString() << std::endl;
    return 65;
  }

  int exstatus = 0;
  google::protobuf::util::MessageDifferencer diffr;
  if (!diffr.Compare(*lhs_message, *rhs_message)) {
    exstatus = 1;
    std::string diff_string;
    diffr.ReportDifferencesToString(&diff_string);
    std::cerr << diff_string << std::endl;
  }
  google::protobuf::ShutdownProtobufLibrary();
  return exstatus;
}
