/******************************************************************************
 *
 * (c) 2020 Copyright, Real-Time Innovations, Inc. (RTI)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef RMW_CONNEXT__DISCOVERY_HPP_
#define RMW_CONNEXT__DISCOVERY_HPP_

#include "rmw_connextdds_cpp/context.hpp"

rmw_ret_t
rmw_connextdds_discovery_thread_start(rmw_context_impl_t * ctx);

rmw_ret_t
rmw_connextdds_discovery_thread_stop(rmw_context_impl_t * ctx);

DDS_DataReader *
rmw_connextdds_get_builtin_reader_participants(rmw_context_impl_t * ctx);

DDS_DataReader *
rmw_connextdds_get_builtin_reader_publications(rmw_context_impl_t * ctx);

DDS_DataReader *
rmw_connextdds_get_builtin_reader_subscriptions(rmw_context_impl_t * ctx);

rmw_ret_t
rmw_connextdds_enable_builtin_readers(rmw_context_impl_t * ctx);

#endif /* RMW_CONNEXT__DISCOVERY_HPP_ */