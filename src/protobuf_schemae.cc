#include "protobuf_schemae.hh"

#include <fstream>
#include <iostream>

#include <fildesh/fildesh.h>
#include <fildesh/ifstream.hh>
#include <fildesh/ofstream.hh>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/message_lite.h>

using google::protobuf::Descriptor;
using google::protobuf::DescriptorDatabase;
using google::protobuf::DescriptorPool;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::FileDescriptorProto;
using google::protobuf::FileDescriptorSet;
using google::protobuf::MergedDescriptorDatabase;
using google::protobuf::Message;
using google::protobuf::SimpleDescriptorDatabase;


static
  std::unique_ptr<SimpleDescriptorDatabase>
PopulateSingleSimpleDescriptorDatabase(const std::string& filename)
{
  FileDescriptorSet fdset;
  fildesh::ifstream in(filename);
  if (!fdset.ParseFromIstream(&in)) {
    std::cerr << "Cannot parse descriptor set file: " << filename << std::endl;
    return nullptr;
  }
  std::unique_ptr<SimpleDescriptorDatabase> database(
      new SimpleDescriptorDatabase());

  for (int i = 0; i < fdset.file_size(); i++) {
    FileDescriptorProto prev;
    if (database->FindFileByName(fdset.file(i).name(), &prev)) {
      continue;  // Skip.
    }
    if (!database->Add(fdset.file(i))) {
      std::cerr << "Failed to add descriptor to database." << std::endl;
      return nullptr;
    }
  }
  return database;
}

  std::unique_ptr<ProtobufSchemae>
ProtobufSchemae::from_descriptor_files(const std::vector<std::string>& filenames)
{
  std::unique_ptr<ProtobufSchemae> schemae(new ProtobufSchemae);
  const unsigned descriptor_count = static_cast<unsigned>(filenames.size());
  schemae->simple_descriptor_dbs_.resize(descriptor_count);
  std::vector<DescriptorDatabase*> raw_descriptor_dbs(descriptor_count);
  for (unsigned i = 0; i < descriptor_count; ++i) {
    schemae->simple_descriptor_dbs_[i] = PopulateSingleSimpleDescriptorDatabase(
        filenames[i]);
    if (!schemae->simple_descriptor_dbs_[i]) {
      return nullptr;
    }
    raw_descriptor_dbs[i] = schemae->simple_descriptor_dbs_[i].get();
  }

  schemae->descriptor_database_.reset(
      new MergedDescriptorDatabase(raw_descriptor_dbs));
  raw_descriptor_dbs.clear();  // Not needed.

  schemae->descriptor_pool_.reset(
      new DescriptorPool(schemae->descriptor_database_.get()));
  schemae->message_factory_.reset(
      new DynamicMessageFactory(schemae->descriptor_pool_.get()));

  return schemae;
}

  std::unique_ptr<google::protobuf::Message>
ProtobufSchemae::new_message(const std::string& message_name)
{
  const Descriptor* message_type =
    this->descriptor_pool_->FindMessageTypeByName(message_name);
  if (!message_type) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return NULL;
  }
  std::unique_ptr<google::protobuf::Message> result(
      this->message_factory_->GetPrototype(message_type)->New());
  return result;
}

  std::unique_ptr<google::protobuf::Message>
ProtobufSchemae::new_message_from_binary_file(
    const std::string& filename,
    const std::string& message_name)
{
  std::unique_ptr<Message> message = this->new_message(message_name);
  if (!message) {
    std::cerr << "Cannot find message type: " << message_name << std::endl;
    return nullptr;
  }

  // Open.
  fildesh::ifstream in(filename);
  if (!in) {
    std::cerr << "Cannot open input file: " << filename << std::endl;
    return nullptr;
  }

  // Read & Parse.
  if (!message->ParseFromIstream(&in)) {
    std::cerr << message->DebugString() << std::endl;
    return nullptr;
  }
  return message;
}

  bool
ProtobufSchemae::write_message_to_binary_file(
    const std::string& filename,
    const google::protobuf::Message& message)
{
  // Open.
  fildesh::ofstream out(filename);
  if (!out) {
    std::cerr << "Error opening output file: " << filename << std::endl;
    return false;
  }

  // Encode & Write.
  if (!message.SerializeToOstream(&out)) {
    std::cerr << "Error encoding binaryproto." << std::endl;
    return false;
  }
  return true;
}

