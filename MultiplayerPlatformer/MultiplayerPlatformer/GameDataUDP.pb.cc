// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: GameDataUDP.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "GameDataUDP.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace GameDataUDP {

namespace {

const ::google::protobuf::Descriptor* PositionUpdate_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  PositionUpdate_reflection_ = NULL;
const ::google::protobuf::Descriptor* DataMessage_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DataMessage_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_GameDataUDP_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_GameDataUDP_2eproto() {
  protobuf_AddDesc_GameDataUDP_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "GameDataUDP.proto");
  GOOGLE_CHECK(file != NULL);
  PositionUpdate_descriptor_ = file->message_type(0);
  static const int PositionUpdate_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PositionUpdate, xpos_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PositionUpdate, ypos_),
  };
  PositionUpdate_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      PositionUpdate_descriptor_,
      PositionUpdate::default_instance_,
      PositionUpdate_offsets_,
      -1,
      -1,
      -1,
      sizeof(PositionUpdate),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PositionUpdate, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PositionUpdate, _is_default_instance_));
  DataMessage_descriptor_ = file->message_type(1);
  static const int DataMessage_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataMessage, positionupdate_),
  };
  DataMessage_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      DataMessage_descriptor_,
      DataMessage::default_instance_,
      DataMessage_offsets_,
      -1,
      -1,
      -1,
      sizeof(DataMessage),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataMessage, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataMessage, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_GameDataUDP_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      PositionUpdate_descriptor_, &PositionUpdate::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      DataMessage_descriptor_, &DataMessage::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_GameDataUDP_2eproto() {
  delete PositionUpdate::default_instance_;
  delete PositionUpdate_reflection_;
  delete DataMessage::default_instance_;
  delete DataMessage_reflection_;
}

void protobuf_AddDesc_GameDataUDP_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AddDesc_GameDataUDP_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021GameDataUDP.proto\022\013GameDataUDP\",\n\016Posi"
    "tionUpdate\022\014\n\004xPos\030\001 \001(\002\022\014\n\004yPos\030\002 \001(\002\"B"
    "\n\013DataMessage\0223\n\016positionUpdate\030\001 \001(\0132\033."
    "GameDataUDP.PositionUpdateb\006proto3", 154);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "GameDataUDP.proto", &protobuf_RegisterTypes);
  PositionUpdate::default_instance_ = new PositionUpdate();
  DataMessage::default_instance_ = new DataMessage();
  PositionUpdate::default_instance_->InitAsDefaultInstance();
  DataMessage::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_GameDataUDP_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_GameDataUDP_2eproto {
  StaticDescriptorInitializer_GameDataUDP_2eproto() {
    protobuf_AddDesc_GameDataUDP_2eproto();
  }
} static_descriptor_initializer_GameDataUDP_2eproto_;

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int PositionUpdate::kXPosFieldNumber;
const int PositionUpdate::kYPosFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

PositionUpdate::PositionUpdate()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:GameDataUDP.PositionUpdate)
}

void PositionUpdate::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

PositionUpdate::PositionUpdate(const PositionUpdate& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:GameDataUDP.PositionUpdate)
}

void PositionUpdate::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  xpos_ = 0;
  ypos_ = 0;
}

PositionUpdate::~PositionUpdate() {
  // @@protoc_insertion_point(destructor:GameDataUDP.PositionUpdate)
  SharedDtor();
}

void PositionUpdate::SharedDtor() {
  if (this != default_instance_) {
  }
}

void PositionUpdate::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* PositionUpdate::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return PositionUpdate_descriptor_;
}

const PositionUpdate& PositionUpdate::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_GameDataUDP_2eproto();
  return *default_instance_;
}

PositionUpdate* PositionUpdate::default_instance_ = NULL;

PositionUpdate* PositionUpdate::New(::google::protobuf::Arena* arena) const {
  PositionUpdate* n = new PositionUpdate;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void PositionUpdate::Clear() {
// @@protoc_insertion_point(message_clear_start:GameDataUDP.PositionUpdate)
#if defined(__clang__)
#define ZR_HELPER_(f) \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Winvalid-offsetof\"") \
  __builtin_offsetof(PositionUpdate, f) \
  _Pragma("clang diagnostic pop")
#else
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<PositionUpdate*>(16)->f)
#endif

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  ZR_(xpos_, ypos_);

#undef ZR_HELPER_
#undef ZR_

}

bool PositionUpdate::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:GameDataUDP.PositionUpdate)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional float xPos = 1;
      case 1: {
        if (tag == 13) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &xpos_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(21)) goto parse_yPos;
        break;
      }

      // optional float yPos = 2;
      case 2: {
        if (tag == 21) {
         parse_yPos:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &ypos_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:GameDataUDP.PositionUpdate)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:GameDataUDP.PositionUpdate)
  return false;
#undef DO_
}

void PositionUpdate::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:GameDataUDP.PositionUpdate)
  // optional float xPos = 1;
  if (this->xpos() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(1, this->xpos(), output);
  }

  // optional float yPos = 2;
  if (this->ypos() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(2, this->ypos(), output);
  }

  // @@protoc_insertion_point(serialize_end:GameDataUDP.PositionUpdate)
}

::google::protobuf::uint8* PositionUpdate::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:GameDataUDP.PositionUpdate)
  // optional float xPos = 1;
  if (this->xpos() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(1, this->xpos(), target);
  }

  // optional float yPos = 2;
  if (this->ypos() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(2, this->ypos(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:GameDataUDP.PositionUpdate)
  return target;
}

int PositionUpdate::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:GameDataUDP.PositionUpdate)
  int total_size = 0;

  // optional float xPos = 1;
  if (this->xpos() != 0) {
    total_size += 1 + 4;
  }

  // optional float yPos = 2;
  if (this->ypos() != 0) {
    total_size += 1 + 4;
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void PositionUpdate::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:GameDataUDP.PositionUpdate)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const PositionUpdate* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const PositionUpdate>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:GameDataUDP.PositionUpdate)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:GameDataUDP.PositionUpdate)
    MergeFrom(*source);
  }
}

void PositionUpdate::MergeFrom(const PositionUpdate& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:GameDataUDP.PositionUpdate)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from.xpos() != 0) {
    set_xpos(from.xpos());
  }
  if (from.ypos() != 0) {
    set_ypos(from.ypos());
  }
}

void PositionUpdate::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:GameDataUDP.PositionUpdate)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void PositionUpdate::CopyFrom(const PositionUpdate& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:GameDataUDP.PositionUpdate)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PositionUpdate::IsInitialized() const {

  return true;
}

void PositionUpdate::Swap(PositionUpdate* other) {
  if (other == this) return;
  InternalSwap(other);
}
void PositionUpdate::InternalSwap(PositionUpdate* other) {
  std::swap(xpos_, other->xpos_);
  std::swap(ypos_, other->ypos_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata PositionUpdate::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = PositionUpdate_descriptor_;
  metadata.reflection = PositionUpdate_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// PositionUpdate

// optional float xPos = 1;
void PositionUpdate::clear_xpos() {
  xpos_ = 0;
}
 float PositionUpdate::xpos() const {
  // @@protoc_insertion_point(field_get:GameDataUDP.PositionUpdate.xPos)
  return xpos_;
}
 void PositionUpdate::set_xpos(float value) {
  
  xpos_ = value;
  // @@protoc_insertion_point(field_set:GameDataUDP.PositionUpdate.xPos)
}

// optional float yPos = 2;
void PositionUpdate::clear_ypos() {
  ypos_ = 0;
}
 float PositionUpdate::ypos() const {
  // @@protoc_insertion_point(field_get:GameDataUDP.PositionUpdate.yPos)
  return ypos_;
}
 void PositionUpdate::set_ypos(float value) {
  
  ypos_ = value;
  // @@protoc_insertion_point(field_set:GameDataUDP.PositionUpdate.yPos)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int DataMessage::kPositionUpdateFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

DataMessage::DataMessage()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:GameDataUDP.DataMessage)
}

void DataMessage::InitAsDefaultInstance() {
  _is_default_instance_ = true;
  positionupdate_ = const_cast< ::GameDataUDP::PositionUpdate*>(&::GameDataUDP::PositionUpdate::default_instance());
}

DataMessage::DataMessage(const DataMessage& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:GameDataUDP.DataMessage)
}

void DataMessage::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  positionupdate_ = NULL;
}

DataMessage::~DataMessage() {
  // @@protoc_insertion_point(destructor:GameDataUDP.DataMessage)
  SharedDtor();
}

void DataMessage::SharedDtor() {
  if (this != default_instance_) {
    delete positionupdate_;
  }
}

void DataMessage::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DataMessage::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DataMessage_descriptor_;
}

const DataMessage& DataMessage::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_GameDataUDP_2eproto();
  return *default_instance_;
}

DataMessage* DataMessage::default_instance_ = NULL;

DataMessage* DataMessage::New(::google::protobuf::Arena* arena) const {
  DataMessage* n = new DataMessage;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void DataMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:GameDataUDP.DataMessage)
  if (GetArenaNoVirtual() == NULL && positionupdate_ != NULL) delete positionupdate_;
  positionupdate_ = NULL;
}

bool DataMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:GameDataUDP.DataMessage)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .GameDataUDP.PositionUpdate positionUpdate = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_positionupdate()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:GameDataUDP.DataMessage)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:GameDataUDP.DataMessage)
  return false;
#undef DO_
}

void DataMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:GameDataUDP.DataMessage)
  // optional .GameDataUDP.PositionUpdate positionUpdate = 1;
  if (this->has_positionupdate()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, *this->positionupdate_, output);
  }

  // @@protoc_insertion_point(serialize_end:GameDataUDP.DataMessage)
}

::google::protobuf::uint8* DataMessage::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:GameDataUDP.DataMessage)
  // optional .GameDataUDP.PositionUpdate positionUpdate = 1;
  if (this->has_positionupdate()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        1, *this->positionupdate_, false, target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:GameDataUDP.DataMessage)
  return target;
}

int DataMessage::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:GameDataUDP.DataMessage)
  int total_size = 0;

  // optional .GameDataUDP.PositionUpdate positionUpdate = 1;
  if (this->has_positionupdate()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->positionupdate_);
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DataMessage::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:GameDataUDP.DataMessage)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const DataMessage* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const DataMessage>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:GameDataUDP.DataMessage)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:GameDataUDP.DataMessage)
    MergeFrom(*source);
  }
}

void DataMessage::MergeFrom(const DataMessage& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:GameDataUDP.DataMessage)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from.has_positionupdate()) {
    mutable_positionupdate()->::GameDataUDP::PositionUpdate::MergeFrom(from.positionupdate());
  }
}

void DataMessage::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:GameDataUDP.DataMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DataMessage::CopyFrom(const DataMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:GameDataUDP.DataMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DataMessage::IsInitialized() const {

  return true;
}

void DataMessage::Swap(DataMessage* other) {
  if (other == this) return;
  InternalSwap(other);
}
void DataMessage::InternalSwap(DataMessage* other) {
  std::swap(positionupdate_, other->positionupdate_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata DataMessage::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DataMessage_descriptor_;
  metadata.reflection = DataMessage_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// DataMessage

// optional .GameDataUDP.PositionUpdate positionUpdate = 1;
bool DataMessage::has_positionupdate() const {
  return !_is_default_instance_ && positionupdate_ != NULL;
}
void DataMessage::clear_positionupdate() {
  if (GetArenaNoVirtual() == NULL && positionupdate_ != NULL) delete positionupdate_;
  positionupdate_ = NULL;
}
const ::GameDataUDP::PositionUpdate& DataMessage::positionupdate() const {
  // @@protoc_insertion_point(field_get:GameDataUDP.DataMessage.positionUpdate)
  return positionupdate_ != NULL ? *positionupdate_ : *default_instance_->positionupdate_;
}
::GameDataUDP::PositionUpdate* DataMessage::mutable_positionupdate() {
  
  if (positionupdate_ == NULL) {
    positionupdate_ = new ::GameDataUDP::PositionUpdate;
  }
  // @@protoc_insertion_point(field_mutable:GameDataUDP.DataMessage.positionUpdate)
  return positionupdate_;
}
::GameDataUDP::PositionUpdate* DataMessage::release_positionupdate() {
  // @@protoc_insertion_point(field_release:GameDataUDP.DataMessage.positionUpdate)
  
  ::GameDataUDP::PositionUpdate* temp = positionupdate_;
  positionupdate_ = NULL;
  return temp;
}
void DataMessage::set_allocated_positionupdate(::GameDataUDP::PositionUpdate* positionupdate) {
  delete positionupdate_;
  positionupdate_ = positionupdate;
  if (positionupdate) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:GameDataUDP.DataMessage.positionUpdate)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace GameDataUDP

// @@protoc_insertion_point(global_scope)