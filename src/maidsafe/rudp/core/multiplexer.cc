/*******************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of MaidSafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of MaidSafe.net. *
 ******************************************************************************/
// Original author: Christopher M. Kohlhoff (chris at kohlhoff dot com)


#include "maidsafe/rudp/core/multiplexer.h"

#include <cassert>

#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/rudp/packets/packet.h"
#include "maidsafe/rudp/utils.h"


namespace asio = boost::asio;
namespace ip = boost::asio::ip;
namespace bs = boost::system;

namespace maidsafe {

namespace rudp {

namespace detail {

Multiplexer::Multiplexer(asio::io_service& asio_service)  // NOLINT
  : socket_(asio_service),
    receive_buffer_(Parameters::max_size),
    sender_endpoint_(),
    dispatcher_(),
    external_endpoint_(),
    best_guess_external_endpoint_(),
    mutex_() {}

ReturnCode Multiplexer::Open(const ip::udp::endpoint& endpoint) {
  if (socket_.is_open()) {
    LOG(kWarning) << "Multiplexer already open.";
    return kAlreadyStarted;
  }

  assert(!endpoint.address().is_unspecified());

  bs::error_code ec;
  socket_.open(endpoint.protocol(), ec);

  if (ec) {
    LOG(kError) << "Multiplexer socket opening error while attempting on " << endpoint
                << "  Error: " << ec.message();
    return kInvalidAddress;
  }

  ip::udp::socket::non_blocking_io nbio(true);
  socket_.io_control(nbio, ec);

  if (ec) {
    LOG(kError) << "Multiplexer setting option error while attempting on " << endpoint
                << "  Error: " << ec.message();
    return kSetOptionFailure;
  }

  if (endpoint.port() == 0U) {
    // Try to bind to Resilience port first. If this fails, just fall back to port 0 (i.e. any port)
    socket_.bind(ip::udp::endpoint(endpoint.address(), ManagedConnections::kResiliencePort()), ec);
    if (!ec)
      return kSuccess;
  }

  socket_.bind(endpoint, ec);
  if (ec) {
    LOG(kError) << "Multiplexer socket binding error while attempting on " << endpoint
                << "  Error: " << ec.value();
    return kBindError;
  }

  return kSuccess;
}

bool Multiplexer::IsOpen() const {
  return socket_.is_open();
}

void Multiplexer::Close() {
  bs::error_code ec;
  socket_.close(ec);
  if (ec)
    LOG(kWarning) << "Multiplexer closing error: " << ec.message();
  assert(!IsOpen());
  std::lock_guard<std::mutex> lock(mutex_);
  external_endpoint_ = ip::udp::endpoint();
  best_guess_external_endpoint_ = ip::udp::endpoint();
}

ip::udp::endpoint Multiplexer::local_endpoint() const {
  boost::system::error_code ec;
  ip::udp::endpoint local_endpoint(socket_.local_endpoint(ec));
  if (ec) {
    if (IsOpen())
      LOG(kError) << ec.message();
    return ip::udp::endpoint();
  }
  return local_endpoint;
}

ip::udp::endpoint Multiplexer::external_endpoint() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return IsValid(external_endpoint_) ? external_endpoint_ : best_guess_external_endpoint_;
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

