// generated from rosidl_typesupport_microxrcedds_cpp/resource/idl__type_support.cpp.em
// with input from visualization_msgs:msg/InteractiveMarkerInit.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_init__rosidl_typesupport_microxrcedds_cpp.hpp"
#include "visualization_msgs/msg/detail/interactive_marker_init__struct.hpp"

#include <limits>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstring>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_microxrcedds_cpp/identifier.hpp"
#include "rosidl_typesupport_microxrcedds_c/message_type_support.h"
#include "rosidl_typesupport_microxrcedds_cpp/message_type_support_decl.hpp"
#include "ucdr/microcdr.h"

#define MICROXRCEDDS_PADDING sizeof(uint32_t)

// forward declaration of message dependencies and their conversion functions
namespace visualization_msgs
{
namespace msg
{
namespace typesupport_microxrcedds_cpp
{
bool cdr_serialize(
  const visualization_msgs::msg::InteractiveMarker &,
  ucdrBuffer *);

bool cdr_deserialize(
  ucdrBuffer *,
  visualization_msgs::msg::InteractiveMarker &);

size_t get_serialized_size(
  const visualization_msgs::msg::InteractiveMarker &,
  size_t current_alignment);

size_t
max_serialized_size_InteractiveMarker(
  bool * full_bounded,
  size_t current_alignment);
}  // namespace typesupport_microxrcedds_cpp
}  // namespace msg
}  // namespace visualization_msgs


namespace visualization_msgs
{

namespace msg
{

namespace typesupport_microxrcedds_cpp
{

bool
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_visualization_msgs
cdr_serialize(
  const visualization_msgs::msg::InteractiveMarkerInit & ros_message,
  ucdrBuffer * cdr)
{
  (void) ros_message;
  (void) cdr;
  bool rv = false;

  // Member: server_id
  rv = ucdr_serialize_string(cdr, ros_message.server_id.c_str());
  // Member: seq_num
  rv = ucdr_serialize_uint64_t(cdr, ros_message.seq_num);
  // Member: markers
  {
    size_t size = ros_message.markers.size();
    rv = ucdr_serialize_uint32_t(cdr, size);

    size_t i = 0;
    while (i < size && rv) {
      rv = visualization_msgs::msg::typesupport_microxrcedds_cpp::cdr_serialize(
        ros_message.markers[i],
        cdr);
      i++;
    }
  }

  return rv;
}

bool
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_visualization_msgs
cdr_deserialize(
  ucdrBuffer * cdr,
  visualization_msgs::msg::InteractiveMarkerInit & ros_message)
{
  (void) cdr;
  (void) ros_message;
  bool rv = false;

  // Member: server_id
  ros_message.server_id.resize(ros_message.server_id.capacity());
  rv = ucdr_deserialize_string(cdr, &ros_message.server_id[0], ros_message.server_id.capacity());
  if (rv) {
    ros_message.server_id.resize(std::strlen(&ros_message.server_id[0]));
  }
  // Member: seq_num
  rv = ucdr_deserialize_uint64_t(cdr, &ros_message.seq_num);
  // Member: markers
  {
    uint32_t size;
    rv = ucdr_deserialize_uint32_t(cdr, &size);
    ros_message.markers.resize(size);

    size_t i = 0;
    while (i < size && rv) {
      rv = visualization_msgs::msg::typesupport_microxrcedds_cpp::cdr_deserialize(
        cdr,
        ros_message.markers[i]);
      i++;
    }
  }

  return rv;
}

size_t
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_visualization_msgs
get_serialized_size(
  const visualization_msgs::msg::InteractiveMarkerInit & ros_message,
  size_t current_alignment)
{
  (void) current_alignment;
  (void) ros_message;

  const size_t initial_alignment = current_alignment;

  // Member: server_id
  current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
  current_alignment += ros_message.server_id.size() + 1;
  // Member: seq_num
  {
    const size_t item_size = sizeof(ros_message.seq_num);
    current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
  }
  // Member: markers
  {
  // Member is abstractsequence
    const size_t sequence_size = ros_message.markers.size();
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    for (size_t i = 0; i < sequence_size; i++) {
      const size_t item_size = visualization_msgs::msg::typesupport_microxrcedds_cpp::get_serialized_size(
        ros_message.markers[i],
        current_alignment);
      current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
    }
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_visualization_msgs
max_serialized_size_InteractiveMarkerInit(
  bool * full_bounded,
  size_t current_alignment)
{
  (void) current_alignment;
  *full_bounded = true;

  const size_t initial_alignment = current_alignment;

  // Member: server_id
  *full_bounded = false;
  // Member: seq_num
  current_alignment += ucdr_alignment(current_alignment, sizeof(uint64_t)) + sizeof(uint64_t);
  // Member: markers
  {
    *full_bounded = false;
  }

  return current_alignment - initial_alignment;
}

static bool _InteractiveMarkerInit__cdr_serialize(
  const void * untyped_ros_message,
  ucdrBuffer * cdr)
{
  auto typed_message =
    static_cast<const visualization_msgs::msg::InteractiveMarkerInit *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _InteractiveMarkerInit__cdr_deserialize(
  ucdrBuffer * cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<visualization_msgs::msg::InteractiveMarkerInit *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _InteractiveMarkerInit__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const visualization_msgs::msg::InteractiveMarkerInit *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _InteractiveMarkerInit__get_serialized_size_with_initial_alignment(
  const void * untyped_ros_message, size_t current_alignment)
{
  auto typed_message =
    static_cast<const visualization_msgs::msg::InteractiveMarkerInit *>(
    untyped_ros_message);
  return static_cast<size_t>(get_serialized_size(*typed_message, current_alignment));
}

static size_t _InteractiveMarkerInit__max_serialized_size()
{
  bool full_bounded;
  return max_serialized_size_InteractiveMarkerInit(&full_bounded, 0);
}

static message_type_support_callbacks_t _InteractiveMarkerInit__callbacks = {
  "visualization_msgs::msg",
  "InteractiveMarkerInit",
  _InteractiveMarkerInit__cdr_serialize,
  _InteractiveMarkerInit__cdr_deserialize,
  _InteractiveMarkerInit__get_serialized_size,
  _InteractiveMarkerInit__get_serialized_size_with_initial_alignment,
  _InteractiveMarkerInit__max_serialized_size
};

static rosidl_message_type_support_t _InteractiveMarkerInit__handle = {
  rosidl_typesupport_microxrcedds_cpp::typesupport_identifier,
  &_InteractiveMarkerInit__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_microxrcedds_cpp

}  // namespace msg

}  // namespace visualization_msgs

namespace rosidl_typesupport_microxrcedds_cpp
{

template<>
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
get_message_type_support_handle<visualization_msgs::msg::InteractiveMarkerInit>()
{
  return &visualization_msgs::msg::typesupport_microxrcedds_cpp::_InteractiveMarkerInit__handle;
}

}  // namespace rosidl_typesupport_microxrcedds_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_cpp, visualization_msgs, msg, InteractiveMarkerInit)() {
  return &visualization_msgs::msg::typesupport_microxrcedds_cpp::_InteractiveMarkerInit__handle;
}

#ifdef __cplusplus
}
#endif
