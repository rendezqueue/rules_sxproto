
#include <iostream>

#include <fildesh/fildesh.h>
#include <google/protobuf/util/message_differencer.h>

#include "protobuf_schemae.hh"
#include "protobuf_transcode.hh"

using google::protobuf::Message;


static bool str_ends_with(const std::string& a, const std::string& sfx) {
  if (sfx.size() > a.size()) {
    return false;
  }
  return (0 == a.compare(a.size()-sfx.size(), sfx.size(), sfx));
}

static
  std::unique_ptr<Message>
new_message_from_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  if (str_ends_with(in_filename, ".sxproto")) {
    return new_message_from_sxproto_file(in_filename, message_name, schemae);
  }
  if (str_ends_with(in_filename, ".textproto")) {
    return new_message_from_textproto_file(in_filename, message_name, schemae);
  }
  if (str_ends_with(in_filename, ".json")) {
    return new_message_from_json_file(in_filename, message_name, schemae);
  }
  return schemae.new_message_from_binary_file(in_filename, message_name);
}

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

  std::unique_ptr<Message> lhs_message = (
      new_message_from_file(argv[1], message_name, *schemae));
  std::unique_ptr<Message> rhs_message = (
      new_message_from_file(argv[2], message_name, *schemae));
  if (!lhs_message || !rhs_message) {
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
