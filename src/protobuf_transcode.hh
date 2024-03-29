#ifndef SXPROTO_PROTOBUF_TRANSCODE_HH_
#define SXPROTO_PROTOBUF_TRANSCODE_HH_

#include <string>
#include <memory>

#include <google/protobuf/message.h>

class ProtobufSchemae;

std::unique_ptr<google::protobuf::Message>
new_message_from_sxproto_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae);

std::unique_ptr<google::protobuf::Message>
new_message_from_textproto_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae);

std::unique_ptr<google::protobuf::Message>
new_message_from_json_file(
    const std::string& in_filename,
    const std::string& message_name,
    ProtobufSchemae& schemae);

bool
write_message_to_json_file(
    const std::string& out_filename,
    const google::protobuf::Message& message,
    bool camelcase=false);
bool
write_message_to_json_camelcase_file(
    const std::string& out_filename,
    const google::protobuf::Message& message);

#endif
