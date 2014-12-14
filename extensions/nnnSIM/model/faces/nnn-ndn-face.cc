/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-ndn-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-ndn-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-ndn-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/uinteger.h>


#include <ns3-dev/ns3/ndn-header-helper.h>
#include <ns3-dev/ns3/ndn-wire.h>

#include "../../helper/nnn-header-helper.h"
#include "../nnn-nnnsim-wire.h"

#include "nnn-ndn-face.h"

NS_LOG_COMPONENT_DEFINE ("nnn.ndnFace");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (NDNFace);

    TypeId
    NDNFace::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::nnn::ndnFace")
        	    .SetParent<nnn::Face> ()
        	    .SetGroupName ("Nnn")
        	    ;
      return tid;
    }

    NDNFace::NDNFace (Ptr<Node> node)
    : ns3::nnn::Face (node)
    , ns3::ndn::Face (node)
    {
      NS_LOG_FUNCTION (this << node);

      NS_ASSERT_MSG (node != 0, "node cannot be NULL. Check the code");

      nnn::Face::SetFlags(nnn::Face::NDN);
    }

    NDNFace::~NDNFace ()
    {
    }

    bool
    NDNFace::ReceiveInterest (Ptr<ndn::Interest> interest)
    {

    }

    bool
    NDNFace::ReceiveData (Ptr<ndn::Data> data)
    {

    }

    bool
    NDNFace::SendInterest (Ptr<const ndn::Interest> interest)
    {

    }


    bool
    NDNFace::SendData (Ptr<const ndn::Data> data)
    {

    }

    void
    NDNFace::insertSO(Ptr<ndn::Name> name, Ptr<NNNAddress> addr)
    {
      ndn_nnn_so_map.insert(std::pair<Ptr<ndn::Name>, Ptr<NNNAddress> > (name, addr) );
    }

    void
    NDNFace::insertDO(Ptr<ndn::Name> name, Ptr<NNNAddress> addr)
    {
      ndn_nnn_so_map.insert(std::pair<Ptr<ndn::Name>, Ptr<NNNAddress> > (name, addr) );
    }

    bool
    NDNFace::Receive(Ptr<Packet> packet)
    {
      NS_LOG_FUNCTION (this);

      // Since this is a NNN-NDN Face, we have to find out what type of header
      // we have just seen. Once found, we pass the packet to the corresponding
      // forwarding strategy

      if (!nnn::Face::IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even
	  // know that something was there
	  return false;
	}

      // Give the next layer a read-write copy of the packet
      Ptr<Packet> cpacket = packet->Copy ();

      // First attempt to match to NNN packets
      try
      {
	  nnn::HeaderHelper::Type type = nnn::HeaderHelper::GetNNNHeaderType (cpacket);
	  switch (type)
	  {
	    case HeaderHelper::NULL_NNN:
	      return ReceiveNULLp (Wire::ToNULLp (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::SO_NNN:
	      return ReceiveSO (Wire::ToSO (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::DO_NNN:
	      return ReceiveDO (Wire::ToDO (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::EN_NNN:
	      return ReceiveEN (Wire::ToEN (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::AEN_NNN:
	      return ReceiveAEN (Wire::ToAEN (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::REN_NNN:
	      return ReceiveREN (Wire::ToREN (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::DEN_NNN:
	      return ReceiveDEN (Wire::ToDEN (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	    case HeaderHelper::INF_NNN:
	      return ReceiveINF (Wire::ToINF (cpacket, Wire::WIRE_FORMAT_NNNSIM));
	  }
	  // exception will be thrown if packet is not recognized
      }
      catch (nnn::UnknownHeaderException)
      {
	  // Means this isn't a NNN packet, switch to attempt NDN matching
      }

      try
      {
	  ndn::HeaderHelper::Type type = ndn::HeaderHelper::GetNdnHeaderType (cpacket);
	  switch (type)
	  {
	    case ndn::HeaderHelper::INTEREST_NDNSIM:
	      return ReceiveInterest (ndn::Wire::ToInterest (cpacket, ndn::Wire::WIRE_FORMAT_NDNSIM));
	    case ndn::HeaderHelper::INTEREST_CCNB:
	      return ReceiveInterest (ndn::Wire::ToInterest (cpacket, ndn::Wire::WIRE_FORMAT_CCNB));
	    case ndn::HeaderHelper::CONTENT_OBJECT_NDNSIM:
	      return ReceiveData (ndn::Wire::ToData (cpacket, ndn::Wire::WIRE_FORMAT_NDNSIM));
	    case ndn::HeaderHelper::CONTENT_OBJECT_CCNB:
	      return ReceiveData (ndn::Wire::ToData (cpacket, ndn::Wire::WIRE_FORMAT_CCNB));
	    default:
	      NS_FATAL_ERROR ("Not supported NDN or NNN header!");
	      return false;
	  }
	  // exception will be thrown if packet is not recognized
      }
      catch (ndn::UnknownHeaderException)
      {
	  NS_FATAL_ERROR ("Unknown NDN or NNN header. Should not happen");
	  return false;
      }

      return false;
    }

    bool
    NDNFace::Send(Ptr<Packet> packet)
    {
      return true;
    }

  } /* namespace nnn */
} /* namespace ns3 */
