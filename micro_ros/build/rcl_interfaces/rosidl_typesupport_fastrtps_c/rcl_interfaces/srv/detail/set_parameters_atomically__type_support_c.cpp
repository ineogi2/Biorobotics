// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from rcl_interfaces:srv/SetParametersAtomically.idl
// generated code does not contain a copyright notice
#include "rcl_interfaces/srv/detail/set_parameters_atomically__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rcl_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "rcl_interfaces/srv/detail/set_parameters_atomically__struct.h"
#include "rcl_interfaces/srv/detail/set_parameters_atomically__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rcl_interfaces/msg/detail/parameter__functions.h"  // parameters

// forward declare type support functions
size_t get_serialized_size_rcl_interfaces__msg__Parameter(
  const void * untyped_ros_message,
  size_t current_alignment);

size_t max_serialized_size_rcl_interfaces__msg__Parameter(
  bool & full_bounded,
  size_t current_alignment);

const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, Parameter)();


using _SetParametersAtomically_Request__ros_msg_type = rcl_interfaces__srv__SetParametersAtomically_Request;

static bool _SetParametersAtomically_Request__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _SetParametersAtomically_Request__ros_msg_type * ros_message = static_cast<const _SetParametersAtomically_Request__ros_msg_type *>(untyped_ros_message);
  // Field name: parameters
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, Parameter
      )()->data);
    size_t size = ros_message->parameters.size;
    auto array_ptr = ros_message->parameters.data;
    cdr << static_cast<uint32_t>(size);
    for (size_t i = 0; i < size; ++i) {
      if (!callbacks->cdr_serialize(
          &array_ptr[i], cdr))
      {
        return false;
      }
    }
  }

  return true;
}

static bool _SetParametersAtomically_Request__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _SetParametersAtomically_Request__ros_msg_type * ros_message = static_cast<_SetParametersAtomically_Request__ros_msg_type *>(untyped_ros_message);
  // Field name: parameters
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, Parameter
      )()->data);
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    if (ros_message->parameters.data) {
      rcl_interfaces__msg__Parameter__Sequence__fini(&ros_message->parameters);
    }
    if (!rcl_interfaces__msg__Parameter__Sequence__init(&ros_message->parameters, size)) {
      return "failed to create array for field 'parameters'";
    }
    auto array_ptr = ros_message->parameters.data;
    for (size_t i = 0; i < size; ++i) {
      if (!callbacks->cdr_deserialize(
          cdr, &array_ptr[i]))
      {
        return false;
      }
    }
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rcl_interfaces
size_t get_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Request(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _SetParametersAtomically_Request__ros_msg_type * ros_message = static_cast<const _SetParametersAtomically_Request__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name parameters
  {
    size_t array_size = ros_message->parameters.size;
    auto array_ptr = ros_message->parameters.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);

    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += get_serialized_size_rcl_interfaces__msg__Parameter(
        &array_ptr[index], current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

static uint32_t _SetParametersAtomically_Request__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Request(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rcl_interfaces
size_t max_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Request(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;

  // member: parameters
  {
    size_t array_size = 0;
    full_bounded = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);


    for (size_t index = 0; index < array_size; ++index) {
      current_alignment +=
        max_serialized_size_rcl_interfaces__msg__Parameter(
        full_bounded, current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

static size_t _SetParametersAtomically_Request__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Request(
    full_bounded, 0);
}


static message_type_support_callbacks_t __callbacks_SetParametersAtomically_Request = {
  "rcl_interfaces::srv",
  "SetParametersAtomically_Request",
  _SetParametersAtomically_Request__cdr_serialize,
  _SetParametersAtomically_Request__cdr_deserialize,
  _SetParametersAtomically_Request__get_serialized_size,
  _SetParametersAtomically_Request__max_serialized_size
};

static rosidl_message_type_support_t _SetParametersAtomically_Request__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_SetParametersAtomically_Request,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, srv, SetParametersAtomically_Request)() {
  return &_SetParametersAtomically_Request__type_support;
}

#if defined(__cplusplus)
}
#endif

// already included above
// #include <cassert>
// already included above
// #include <limits>
// already included above
// #include <string>
// already included above
// #include "rosidl_typesupport_fastrtps_c/identifier.h"
// already included above
// #include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
// already included above
// #include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
// already included above
// #include "rcl_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
// already included above
// #include "rcl_interfaces/srv/detail/set_parameters_atomically__struct.h"
// already included above
// #include "rcl_interfaces/srv/detail/set_parameters_atomically__functions.h"
// already included above
// #include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rcl_interfaces/msg/detail/set_parameters_result__functions.h"  // result

// forward declare type support functions
size_t get_serialized_size_rcl_interfaces__msg__SetParametersResult(
  const void * untyped_ros_message,
  size_t current_alignment);

size_t max_serialized_size_rcl_interfaces__msg__SetParametersResult(
  bool & full_bounded,
  size_t current_alignment);

const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, SetParametersResult)();


using _SetParametersAtomically_Response__ros_msg_type = rcl_interfaces__srv__SetParametersAtomically_Response;

static bool _SetParametersAtomically_Response__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _SetParametersAtomically_Response__ros_msg_type * ros_message = static_cast<const _SetParametersAtomically_Response__ros_msg_type *>(untyped_ros_message);
  // Field name: result
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, SetParametersResult
      )()->data);
    if (!callbacks->cdr_serialize(
        &ros_message->result, cdr))
    {
      return false;
    }
  }

  return true;
}

static bool _SetParametersAtomically_Response__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _SetParametersAtomically_Response__ros_msg_type * ros_message = static_cast<_SetParametersAtomically_Response__ros_msg_type *>(untyped_ros_message);
  // Field name: result
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, rcl_interfaces, msg, SetParametersResult
      )()->data);
    if (!callbacks->cdr_deserialize(
        cdr, &ros_message->result))
    {
      return false;
    }
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rcl_interfaces
size_t get_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Response(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _SetParametersAtomically_Response__ros_msg_type * ros_message = static_cast<const _SetParametersAtomically_Response__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name result

  current_alignment += get_serialized_size_rcl_interfaces__msg__SetParametersResult(
    &(ros_message->result), current_alignment);

  return current_alignment - initial_alignment;
}

static uint32_t _SetParametersAtomically_Response__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Response(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_rcl_interfaces
size_t max_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Response(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;

  // member: result
  {
    size_t array_size = 1;


    for (size_t index = 0; index < array_size; ++index) {
      current_alignment +=
        max_serialized_size_rcl_interfaces__msg__SetParametersResult(
        full_bounded, current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

static size_t _SetParametersAtomically_Response__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_rcl_interfaces__srv__SetParametersAtomically_Response(
    full_bounded, 0);
}


static message_type_support_callbacks_t __callbacks_SetParametersAtomically_Response = {
  "rcl_interfaces::srv",
  "SetParametersAtomically_Response",
  _SetParametersAtomically_Response__cdr_serialize,
  _SetParametersAtomically_Response__cdr_deserialize,
  _SetParametersAtomically_Response__get_serialized_size,
  _SetParametersAtomically_Response__max_serialized_size
};

static rosidl_message_type_support_t _SetParametersAtomically_Response__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_SetParametersAtomically_Response,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, srv, SetParametersAtomically_Response)() {
  return &_SetParametersAtomically_Response__type_support;
}

#if defined(__cplusplus)
}
#endif

#include "rosidl_typesupport_fastrtps_cpp/service_type_support.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_fastrtps_c/identifier.h"
// already included above
// #include "rcl_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "rcl_interfaces/srv/set_parameters_atomically.h"

#if defined(__cplusplus)
extern "C"
{
#endif

static service_type_support_callbacks_t SetParametersAtomically__callbacks = {
  "rcl_interfaces::srv",
  "SetParametersAtomically",
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, srv, SetParametersAtomically_Request)(),
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, srv, SetParametersAtomically_Response)(),
};

static rosidl_service_type_support_t SetParametersAtomically__handle = {
  rosidl_typesupport_fastrtps_c__identifier,
  &SetParametersAtomically__callbacks,
  get_service_typesupport_handle_function,
};

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, rcl_interfaces, srv, SetParametersAtomically)() {
  return &SetParametersAtomically__handle;
}

#if defined(__cplusplus)
}
#endif
