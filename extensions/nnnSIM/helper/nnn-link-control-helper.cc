/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for University of California, Los Angeles by
 *   Alexander Afanasyev and Saran Tarnoi
 *
 *  nnn-link-control-helper.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-link-control-helper.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-link-control-helper.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/point-to-point-net-device.h>
#include <ns3-dev/ns3/point-to-point-channel.h>
#include <ns3-dev/ns3/channel.h>
#include <ns3-dev/ns3/log.h>

#include "nnn-link-control-helper.h"

#include "../model/nnn-l3-protocol.h"
#include "../model/nnn-net-device-face.h"

NS_LOG_COMPONENT_DEFINE ("nnn.LinkControlHelper");

namespace ns3
{
  namespace nnn
  {
    void
    LinkControlHelper::FailLink (Ptr<Node> node1, Ptr<Node> node2)
    {
      NS_LOG_FUNCTION (node1 << node2);

      NS_ASSERT (node1 != 0);
      NS_ASSERT (node2 != 0);

      Ptr<nnn::L3Protocol> ndn1 = node1->GetObject<nnn::L3Protocol> ();
      Ptr<nnn::L3Protocol> ndn2 = node2->GetObject<nnn::L3Protocol> ();

      NS_ASSERT (ndn1 != 0);
      NS_ASSERT (ndn2 != 0);

      for (uint32_t faceId = 0; faceId < ndn1->GetNFaces (); faceId++)
	{
	  Ptr<nnn::NetDeviceFace> ndFace = ndn1->GetFace (faceId)->GetObject<nnn::NetDeviceFace> ();
	  if (ndFace == 0) continue;

	  Ptr<PointToPointNetDevice> nd1 = ndFace->GetNetDevice ()->GetObject<PointToPointNetDevice> ();
	  if (nd1 == 0) continue;

	  Ptr<Channel> channel = nd1->GetChannel ();
	  if (channel == 0) continue;

	  Ptr<PointToPointChannel> ppChannel = DynamicCast<PointToPointChannel> (channel);

	  Ptr<NetDevice> nd2 = ppChannel->GetDevice (0);
	  if (nd2->GetNode () == node1)
	    nd2 = ppChannel->GetDevice (1);

	  if (nd2->GetNode () == node2)
	    {
	      Ptr<nnn::Face> face1 = ndn1->GetFaceByNetDevice (nd1);
	      Ptr<nnn::Face> face2 = ndn2->GetFaceByNetDevice (nd2);

	      face1->SetUp (false);
	      face2->SetUp (false);
	      break;
	    }
	}
    }
    void
    LinkControlHelper::FailLinkByName (const std::string &node1, const std::string &node2)
    {
      FailLink (Names::Find<Node> (node1), Names::Find<Node> (node2));
    }

    void
    LinkControlHelper::UpLink (Ptr<Node> node1, Ptr<Node> node2)
    {
      NS_LOG_FUNCTION (node1 << node2);

      NS_ASSERT (node1 != 0);
      NS_ASSERT (node2 != 0);

      Ptr<nnn::L3Protocol> ndn1 = node1->GetObject<nnn::L3Protocol> ();
      Ptr<nnn::L3Protocol> ndn2 = node2->GetObject<nnn::L3Protocol> ();

      NS_ASSERT (ndn1 != 0);
      NS_ASSERT (ndn2 != 0);

      // iterate over all faces to find the right one
      for (uint32_t faceId = 0; faceId < ndn1->GetNFaces (); faceId++)
	{
	  Ptr<nnn::NetDeviceFace> ndFace = ndn1->GetFace (faceId)->GetObject<nnn::NetDeviceFace> ();
	  if (ndFace == 0) continue;

	  Ptr<PointToPointNetDevice> nd1 = ndFace->GetNetDevice ()->GetObject<PointToPointNetDevice> ();
	  if (nd1 == 0) continue;

	  Ptr<Channel> channel = nd1->GetChannel ();
	  if (channel == 0) continue;

	  Ptr<PointToPointChannel> ppChannel = DynamicCast<PointToPointChannel> (channel);

	  Ptr<NetDevice> nd2 = ppChannel->GetDevice (0);
	  if (nd2->GetNode () == node1)
	    nd2 = ppChannel->GetDevice (1);

	  if (nd2->GetNode () == node2)
	    {
	      Ptr<nnn::Face> face1 = ndn1->GetFaceByNetDevice (nd1);
	      Ptr<nnn::Face> face2 = ndn2->GetFaceByNetDevice (nd2);

	      face1->SetUp (true);
	      face2->SetUp (true);
	      break;
	    }
	}
    }

    void
    LinkControlHelper::UpLinkByName (const std::string &node1, const std::string &node2)
    {
      UpLink (Names::Find<Node> (node1), Names::Find<Node> (node2));
    }

  } /* namespace nnn */
} /* namespace ns3 */
