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

      this->nnn::Face::SetFlags(nnn::Face::NDN);
    }

    NDNFace::~NDNFace ()
    {
    }

    bool
    NDNFace::ReceiveNULLp (Ptr<NULLp> n_i)
    {

    }

    bool
    NDNFace::ReceiveSO (Ptr<SO> so_i)
    {

    }

    bool
    NDNFace::ReceiveDO (Ptr<DO>  do_i)
    {

    }

    bool
    NDNFace::ReceiveEN (Ptr<EN> en_i)
    {

    }

    bool
    NDNFace::ReceiveAEN (Ptr<AEN> aen_i)
    {

    }

    bool
    NDNFace::ReceiveREN (Ptr<REN> ren_i)
    {

    }

    bool
    NDNFace::ReceiveDEN (Ptr<DEN> den_i)
    {

    }

    bool
    NDNFace::ReceiveData (Ptr<ndn::Data> data)
    {

    }

    bool
    NDNFace::ReceiveInterest (Ptr<ndn::Interest> interest)
    {

    }

    bool
    NDNFace::SendNULLp (Ptr<const NULLp> n_o)
    {

    }

    bool
    NDNFace::SendSO (Ptr<const SO> so_o)
    {

    }

    bool
    NDNFace::SendDO (Ptr<const DO> do_o)
    {

    }

    bool
    NDNFace::SendEN (Ptr<const EN> en_o)
    {

    }

    bool
    NDNFace::SendAEN (Ptr<const AEN> aen_o)
    {

    }

    bool
    NDNFace::SendREN (Ptr<const REN> ren_o)
    {

    }

    bool
    NDNFace::SendDEN (Ptr<const DEN> den_o)
    {

    }

    bool
    NDNFace::SendINF (Ptr<const INF> inf_o)
    {

    }

    bool
    NDNFace::ReceiveINF (Ptr<INF> inf_i)
    {

    }

    bool
    NDNFace::SendData(Ptr<const ndn::Data> data)
    {

    }

    bool
    NDNFace::SendInterest(Ptr<const ndn::Interest> interest)
    {

    }

    bool
    NDNFace::Receive(Ptr<Packet> packet)
    {

    }

    bool
    NDNFace::Send(Ptr<Packet> packet)
    {

    }

  } /* namespace nnn */
} /* namespace ns3 */
