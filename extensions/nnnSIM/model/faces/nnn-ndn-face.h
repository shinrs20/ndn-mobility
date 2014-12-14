/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-ndn-face.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-ndn-face.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-ndn-face.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_NDN_FACE_H_
#define NNN_NDN_FACE_H_

#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/traced-callback.h>
#include <ns3-dev/ns3/type-id.h>

#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndn-face.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>

#include "../nnn-face.h"

namespace ns3
{
  namespace nnn
  {

    class NDNFace : public nnn::Face, public ndn::Face
    {
    public:
      static TypeId
      GetTypeId ();

      NDNFace (Ptr<Node> node);

      virtual
      ~NDNFace ();

      bool
      ReceiveInterest (Ptr<ndn::Interest> interest);

      bool
      ReceiveData (Ptr<ndn::Data> data);

      bool
      SendInterest (Ptr<const ndn::Interest> interest);

      bool
      SendData (Ptr<const ndn::Data> data);

      void
      insertSO(Ptr<ndn::Name> name, Ptr<NNNAddress> addr);

      void
      insertDO(Ptr<ndn::Name> name, Ptr<NNNAddress> addr);

    protected:
      bool
      Receive (Ptr<Packet> packet);

      bool
      Send (Ptr<Packet> packet);

      void
      SetFlags (uint32_t flags);

    private:

      std::multimap<Ptr<ndn::Name>, Ptr<NNNAddress> > ndn_nnn_so_map;
      std::multimap<Ptr<ndn::Name>, Ptr<NNNAddress> > ndn_nnn_do_map;

    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NDN_FACE_H_ */
