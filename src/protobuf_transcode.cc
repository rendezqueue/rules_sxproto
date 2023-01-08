#include "protobuf_transcode.hh"

#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

#include "protobuf_schemae.hh"
#include "sxproto2textproto.h"

using google::protobuf::Message;


static
  std::unique_ptr<std::string>
slurp_file(const std::string& in_filename)
{
  FildeshX* in = open_FildeshXF(in_filename.c_str());
  if (!in) {
    std::cerr << "Cannot open input file: " << in_filename << std::endl;
    return nullptr;
  }
  slurp_FildeshX(in);
  std::unique_ptr<std::string> in_content(new std::string(in->at, in->size));
  close_FildeshX(in);
  return in_content;
}

static
  std::unique_ptr<Message>
new_message_from_textproto_content(
    const std::string& in_content,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  using google::protobuf::TextFormat;
  std::unique_ptr<Message> message = schemae.new_message(message_name);
  if (!message) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return nullptr;
  }

  TextFormat::Parser textproto_parser;
  textproto_parser.AllowFieldNumber(true);
  if (!textproto_parser.ParseFromString(in_content, message.get())) {
    std::cerr << "Error parsing textproto." << std::endl;
    return nullptr;
  }
  return message;
}

static
  std::unique_ptr<Message>
new_message_from_json_content(
    const std::string& in_content,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  using google::protobuf::util::JsonStringToMessage;
  std::unique_ptr<Message> message = schemae.new_message(message_name);
  if (!message) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return nullptr;
  }

  google::protobuf::util::Status status = JsonStringToMessage(
      in_content, message.get());
  if (!status.ok()) {
    std::cerr << "Error parsing json." << std::endl;
    return nullptr;
  }
  return message;
}

  std::unique_ptr<Message>
new_message_from_sxproto_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  // Open.
  FildeshX* in = open_FildeshXF(in_filename.c_str());
  if (!in) {
    std::cerr << "Cannot open input file: " << in_filename << std::endl;
    return nullptr;
  }

  // Trancode to temporary textproto.
  FildeshO tmp_out[1] = {DEFAULT_FildeshO};
  if (!sxproto2textproto(in, tmp_out)) {
    close_FildeshO(tmp_out);
    return nullptr;
  }
  // Input is closed at this point.
  in = NULL;

  const std::string in_content(tmp_out->at, tmp_out->size);
  close_FildeshO(tmp_out);

  // Parse.
  return new_message_from_textproto_content(in_content, message_name, schemae);
}

  std::unique_ptr<Message>
new_message_from_textproto_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  // Read.
  const std::unique_ptr<std::string> in_content = slurp_file(in_filename);
  if (!in_content) {return nullptr;}

  // Parse.
  return new_message_from_textproto_content(*in_content, message_name, schemae);
}

  std::unique_ptr<Message>
new_message_from_json_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae)
{
  // Read.
  const std::unique_ptr<std::string> in_content = slurp_file(in_filename);
  if (!in_content) {return nullptr;}

  // Parse.
  return new_message_from_json_content(*in_content, message_name, schemae);
}

