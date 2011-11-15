/* Copyright (c) 2011 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_TRANSPORT_NAT_DETECTION_H_
#define MAIDSAFE_TRANSPORT_NAT_DETECTION_H_

#include <vector>

#include "maidsafe/transport/contact.h"
#include "maidsafe/transport/rudp_connection.h"
#include "maidsafe/transport/nat_detection_rpcs.h"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"

namespace maidsafe {

namespace transport {

class NatDetection {
 public:
  void Detect(const std::vector<Contact>& contacts,
              std::shared_ptr<Transport> transport,
              MessageHandlerPtr message_handler,
              NatType* nat_type,
              TransportDetails* details);
 protected:
  NatDetectionRpcs rpcs_;
  boost::mutex mutex_;
  boost::condition_variable cond_var_;

 private:
  void DetectCallback(NatType* nat_type, TransportDetails* details);
};

}  // namespace transport

}  // namespace maidsafe

#endif  // MAIDSAFE_TRANSPORT_NAT_DETECTION_H_