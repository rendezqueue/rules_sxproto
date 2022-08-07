#ifndef PROTOBUF_SCHEMAE_HH_
#define PROTOBUF_SCHEMAE_HH_
#include <memory>
#include <string>
#include <vector>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>

class ProtobufSchemae {
 private:
  std::vector<std::unique_ptr<google::protobuf::SimpleDescriptorDatabase>> simple_descriptor_dbs_;
  std::unique_ptr<google::protobuf::MergedDescriptorDatabase> descriptor_database_;
  std::unique_ptr<google::protobuf::DescriptorPool> descriptor_pool_;
  std::unique_ptr<google::protobuf::DynamicMessageFactory> message_factory_;

 public:
  static std::unique_ptr<ProtobufSchemae>
  from_descriptor_files(const std::vector<std::string>& filenames);

  std::unique_ptr<google::protobuf::Message>
  new_message(const std::string& message_name);

  std::unique_ptr<google::protobuf::Message>
  new_message_from_binary_file(const std::string& filename,
                               const std::string& message_name);
  static size_t
  write_message_to_binary_file(const std::string& filename,
                               const google::protobuf::Message& message);
};
#endif
