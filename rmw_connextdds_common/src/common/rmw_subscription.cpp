// Copyright 2020 Real-Time Innovations, Inc. (RTI)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rmw_connextdds/rmw_impl.hpp"
#include "rmw_connextdds/graph_cache.hpp"

#include "rmw/validate_full_topic_name.h"

/******************************************************************************
 * Subscription functions
 ******************************************************************************/

extern "C" rmw_ret_t rmw_init_subscription_allocation(
  const rosidl_message_type_support_t * type_support,
#if RMW_CONNEXT_RELEASE <= RMW_CONNEXT_RELEASE_DASHING
  const rosidl_message_bounds_t * message_bounds,
#else
  const rosidl_runtime_c__Sequence__bound * message_bounds,
#endif /* RMW_CONNEXT_RELEASE <= RMW_CONNEXT_RELEASE_DASHING */
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(type_support);
  UNUSED_ARG(message_bounds);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_ERROR;
}

extern "C" rmw_ret_t rmw_fini_subscription_allocation(
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_ERROR;
}


extern "C" rmw_subscription_t * rmw_create_subscription(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
#if RMW_CONNEXT_HAVE_OPTIONS
  const rmw_subscription_options_t * subscription_options
#else
  bool ignore_local_publications
#endif /* RMW_CONNEXT_HAVE_OPTIONS */
)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);
#if RMW_CONNEXT_HAVE_OPTIONS
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription_options, nullptr);
#endif /* RMW_CONNEXT_HAVE_OPTIONS */

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating new subscription: topic=%s",
    topic_name)

  if (0 == strlen(topic_name)) {
    RMW_CONNEXT_LOG_ERROR("empty topic_name provided")
    return nullptr;
  }

  if (!qos_policies->avoid_ros_namespace_conventions) {
    int validation_result = RMW_TOPIC_VALID;
    rmw_ret_t ret = rmw_validate_full_topic_name(
      topic_name, &validation_result, nullptr);
    if (RMW_RET_OK != ret) {
      return nullptr;
    }
    if (RMW_TOPIC_VALID != validation_result) {
      const char * reason =
        rmw_full_topic_name_validation_result_string(
        validation_result);
      UNUSED_ARG(reason);
      RMW_CONNEXT_LOG_ERROR_A("invalid topic name: %s", reason)
      return nullptr;
    }
  }

  rmw_context_impl_t * ctx = node->context->impl;

  rmw_subscription_t * const rmw_sub =
    rmw_connextdds_create_subscriber(
    ctx,
    node,
    ctx->participant,
    ctx->dds_sub,
    type_supports,
    topic_name,
    qos_policies,
#if RMW_CONNEXT_HAVE_OPTIONS
    subscription_options
#else
    ignore_local_publications
#endif /* RMW_CONNEXT_HAVE_OPTIONS */
    );

  if (nullptr == rmw_sub) {
    RMW_CONNEXT_LOG_ERROR("failed to create RMW subscription")
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "new subscription created: topic=%s, sub=%p",
    topic_name, (void *)rmw_sub->data)

  return rmw_sub;
}


extern "C" rmw_ret_t rmw_subscription_count_matched_publishers(
  const rmw_subscription_t * subscription, size_t * publisher_count)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(publisher_count, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  *publisher_count = sub_impl->publications_count();

  return RMW_RET_OK;
}

extern "C" rmw_ret_t rmw_subscription_get_actual_qos(
  const rmw_subscription_t * subscription,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->qos(qos);
}

extern "C" rmw_ret_t rmw_destroy_subscription(
  rmw_node_t * node,
  rmw_subscription_t * subscription)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_context_impl_t * ctx = node->context->impl;

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_subscriber_deleted(ctx, node, sub_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for subscriber")
    return RMW_RET_ERROR;
  }

  return rmw_connextdds_destroy_subscriber(ctx, subscription);
}


extern "C" rmw_ret_t rmw_take(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->take_message(ros_message, nullptr, taken);

  return rc;
}

extern "C" rmw_ret_t rmw_take_with_info(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->take_message(ros_message, message_info, taken);

  return rc;
}

#if RMW_CONNEXT_HAVE_TAKE_SEQ

extern "C" rmw_ret_t rmw_take_sequence(
  const rmw_subscription_t * subscription,
  size_t count,
  rmw_message_sequence_t * message_sequence,
  rmw_message_info_sequence_t * message_info_sequence,
  size_t * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_sequence, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(
    message_info_sequence, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->take(
    message_sequence, message_info_sequence, count, taken);

  return rc;
}

#endif /* RMW_CONNEXT_HAVE_TAKE_SEQ */

extern "C" rmw_ret_t rmw_take_serialized_message(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->take_serialized(serialized_message, nullptr, taken);
}

extern "C" rmw_ret_t rmw_take_serialized_message_with_info(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->take_serialized(serialized_message, message_info, taken);
}

extern "C" rmw_ret_t rmw_take_loaned_message(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  UNUSED_ARG(taken);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}

extern "C" rmw_ret_t rmw_take_loaned_message_with_info(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  UNUSED_ARG(taken);
  UNUSED_ARG(message_info);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}

extern "C" rmw_ret_t rmw_return_loaned_message_from_subscription(
  const rmw_subscription_t * subscription,
  void * loaned_message)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}
