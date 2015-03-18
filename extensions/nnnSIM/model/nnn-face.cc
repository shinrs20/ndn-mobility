/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/ref.hpp>

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/double.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/pointer.h>
#include <ns3-dev/ns3/random-variable.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/uinteger.h>

#include "nnn-face.h"
#include "nnn-pdus.h"
#include "nnn-nnnsim-wire.h"

#include "../helper/nnn-header-helper.h"

NS_LOG_COMPONENT_DEFINE ("nnn.Face");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (Face);

    TypeId
    Face::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::nnn::Face")
	      .SetParent<Object> ()
	      .SetGroupName ("Nnn")
	      .AddAttribute ("Id", "Face id (unique integer for the Nnn stack on this node)",
	                     TypeId::ATTR_GET, // allow only getting it.
	                     UintegerValue (0),
	                     MakeUintegerAccessor (&Face::m_id),
	                     MakeUintegerChecker<uint32_t> ())
	                     ;
      return tid;
    }

    /**
     * By default, Nnn face are created in the "down" state
     *  with no MAC addresses.  Before becoming useable, the user must
     * invoke SetUp on them once an Nnn address and mask have been set.
     */
    Face::Face (Ptr<Node> node)
    : m_node (node)
    , m_upstreamNULLpHandler (MakeNullCallback< void, Ptr<Face>, Ptr<NULLp> > ())
    , m_upstreamSOHandler    (MakeNullCallback< void, Ptr<Face>, Ptr<SO> > ())
    , m_upstreamDOHandler    (MakeNullCallback< void, Ptr<Face>, Ptr<DO> > ())
    , m_upstreamENHandler    (MakeNullCallback< void, Ptr<Face>, Ptr<EN> > ())
    , m_upstreamAENHandler   (MakeNullCallback< void, Ptr<Face>, Ptr<AEN> > ())
    , m_upstreamRENHandler   (MakeNullCallback< void, Ptr<Face>, Ptr<REN> > ())
    , m_upstreamDENHandler	(MakeNullCallback< void, Ptr<Face>, Ptr<DEN> > ())
    , m_upstreamINFHandler   (MakeNullCallback< void, Ptr<Face>, Ptr<INF> > ())
    , m_ifup (false)
    , m_id ((uint32_t)-1)
    , m_metric (0)
    , m_flags (0)
    {
      NS_LOG_FUNCTION (this << node);

      NS_ASSERT_MSG (node != 0, "node cannot be NULL. Check the code");
    }

    Face::~Face ()
    {
      NS_LOG_FUNCTION_NOARGS ();
    }

    Face::Face (const Face &)
    {
    }

    Face& Face::operator= (const Face &)
    {
      return *this;
    }

    Ptr<Node>
    Face::GetNode () const
    {
      return m_node;
    }

    void
    Face::RegisterNNNProtocolHandlers (const NULLpHandler &NULLpHandler, const SOHandler &SOHandler,
                                       const DOHandler &DOHandler, const ENHandler &ENHandler,
                                       const AENHandler &AENHandler, const RENHandler &RENHandler,
                                       const DENHandler &DENHandler, const INFHandler &INFHandler,
				       const DUHandler &DUHandler, const OENHandler &OENHandler)
    {
      NS_LOG_FUNCTION_NOARGS ();

      m_upstreamNULLpHandler = NULLpHandler;
      m_upstreamSOHandler = SOHandler;
      m_upstreamDOHandler = DOHandler;
      m_upstreamENHandler = ENHandler;
      m_upstreamAENHandler = AENHandler;
      m_upstreamRENHandler = RENHandler;
      m_upstreamDENHandler = DENHandler;
      m_upstreamINFHandler = INFHandler;
      m_upstreamDUHandler = DUHandler;
      m_upstreamOENHandler = OENHandler;
    }

    void
    Face::UnRegisterNNNProtocolHandlers ()
    {
      NS_LOG_FUNCTION_NOARGS ();

      m_upstreamNULLpHandler = MakeNullCallback< void, Ptr<Face>, Ptr<NULLp> > ();
      m_upstreamSOHandler = MakeNullCallback< void, Ptr<Face>, Ptr<SO> > ();
      m_upstreamDOHandler = MakeNullCallback< void, Ptr<Face>, Ptr<DO> > ();
      m_upstreamENHandler = MakeNullCallback< void, Ptr<Face>, Ptr<EN> > ();
      m_upstreamAENHandler = MakeNullCallback< void, Ptr<Face>, Ptr<AEN> > ();
      m_upstreamRENHandler = MakeNullCallback< void, Ptr<Face>, Ptr<REN> > ();
      m_upstreamDENHandler = MakeNullCallback< void, Ptr<Face>, Ptr<DEN> > ();
      m_upstreamINFHandler = MakeNullCallback< void, Ptr<Face>, Ptr<INF> > ();
      m_upstreamOENHandler = MakeNullCallback< void, Ptr<Face>, Ptr<OEN> > ();
    }

    bool
    Face::SendNULLp (Ptr<const NULLp> n_o)
    {
      NS_LOG_FUNCTION (this << n_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromNULLp (n_o));
    }

    bool
    Face::SendNULLp (Ptr<const NULLp> n_o, Address addr)
    {
      NS_LOG_FUNCTION (this << n_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromNULLp (n_o), addr);
    }

    bool
    Face::SendSO (Ptr<const SO> so_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << so_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromSO (so_o));
    }

    bool
    Face::SendSO (Ptr<const SO> so_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << so_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromSO (so_o), addr);
    }

    bool
    Face::SendDO (Ptr<const DO> do_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << do_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDO (do_o));
    }

    bool
    Face::SendDO (Ptr<const DO> do_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << do_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDO (do_o), addr);
    }

    bool
    Face::SendDU (Ptr<const DU> du_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << du_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDU (du_o));
    }

    bool
    Face::SendDU (Ptr<const DU> du_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << du_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDU (du_o), addr);
    }

    bool
    Face::SendEN (Ptr<const EN> en_o)
    {
      NS_LOG_FUNCTION (this << en_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromEN (en_o));
    }

    bool
    Face::SendEN (Ptr<const EN> en_o, Address addr)
    {
      NS_LOG_FUNCTION (this << en_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromEN (en_o), addr);
    }

    bool
    Face::SendAEN (Ptr<const AEN> aen_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << aen_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromAEN (aen_o));
    }

    bool
    Face::SendAEN (Ptr<const AEN> aen_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << aen_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromAEN (aen_o), addr);
    }

    bool
    Face::SendREN (Ptr<const REN> ren_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << ren_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromREN (ren_o));
    }

    bool
    Face::SendREN (Ptr<const REN> ren_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << ren_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromREN (ren_o), addr);
    }

    bool
    Face::SendDEN (Ptr<const DEN> den_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << den_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDEN (den_o));
    }

    bool
    Face::SendDEN (Ptr<const DEN> den_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << den_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromDEN (den_o), addr);
    }

    bool
    Face::SendOEN (Ptr<const OEN> oen_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << oen_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromOEN (oen_o));
    }

    bool
    Face::SendOEN (Ptr<const OEN> oen_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << oen_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromOEN (oen_o), addr);
    }

    bool
    Face::SendINF (Ptr<const INF> inf_o)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << inf_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromINF (inf_o));
    }

    bool
    Face::SendINF (Ptr<const INF> inf_o, Address addr)
    {
      NS_LOG_FUNCTION (this << boost::cref (*this) << inf_o);

      if (!IsUp ())
	{
	  return false;
	}

      return Send (Wire::FromINF (inf_o), addr);
    }

    bool
    Face::Send (Ptr<Packet> packet)
    {
      return true;
    }

    bool
    Face::Send (Ptr<Packet> packet, Address addr)
    {
      return true;
    }

    bool
    Face::Receive (Ptr<const Packet> p)
    {
      NS_LOG_FUNCTION (this << p << p->GetSize ());

      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      Ptr<Packet> packet = p->Copy (); // give upper layers a rw copy of the packet
      try
      {
	  NNN_PDU_TYPE type = HeaderHelper::GetNNNHeaderType (packet);
	  switch (type)
	  {
	    case nnn::NULL_NNN:
	      return ReceiveNULLp (Wire::ToNULLp (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::SO_NNN:
	      return ReceiveSO (Wire::ToSO (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::DO_NNN:
	      return ReceiveDO (Wire::ToDO (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::EN_NNN:
	      return ReceiveEN (Wire::ToEN (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::AEN_NNN:
	      return ReceiveAEN (Wire::ToAEN (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::REN_NNN:
	      return ReceiveREN (Wire::ToREN (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::OEN_NNN:
	      return ReceiveOEN (Wire::ToOEN (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::DEN_NNN:
	      return ReceiveDEN (Wire::ToDEN (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::INF_NNN:
	      return ReceiveINF (Wire::ToINF (packet, Wire::WIRE_FORMAT_NNNSIM));
	    case nnn::DU_NNN:
	      return ReceiveDU (Wire::ToDU (packet, Wire::WIRE_FORMAT_NNNSIM));
	    default:
	      NS_FATAL_ERROR ("Not supported NNN header");
	      return false;
	  }

	  // exception will be thrown if packet is not recognized
      }
      catch (UnknownHeaderException)
      {
	  NS_FATAL_ERROR ("Unknown NNN header. Should not happen");
	  return false;
      }

      return false;
    }

    bool
    Face::ReceiveNULLp (Ptr<NULLp> n_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamNULLpHandler (this, n_i);
      return true;
    }

    bool
    Face::ReceiveSO (Ptr<SO> so_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamSOHandler (this, so_i);
      return true;
    }

    bool
    Face::ReceiveDO (Ptr<DO> do_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamDOHandler (this, do_i);
      return true;
    }

    bool
    Face::ReceiveEN (Ptr<EN> en_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamENHandler (this, en_i);
      return true;
    }

    bool
    Face::ReceiveAEN (Ptr<AEN> aen_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamAENHandler (this, aen_i);
      return true;
    }

    bool
    Face::ReceiveREN (Ptr<REN> ren_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamRENHandler (this, ren_i);
      return true;
    }

    bool
    Face::ReceiveDEN (Ptr<DEN> den_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamDENHandler (this, den_i);
      return true;
    }

    bool
    Face::ReceiveOEN (Ptr<OEN> oen_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamOENHandler (this, oen_i);
      return true;
    }

    bool
    Face::ReceiveINF (Ptr<INF> inf_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamINFHandler (this, inf_i);
      return true;
    }

    bool
    Face::ReceiveDU (Ptr<DU> du_i)
    {
      if (!IsUp ())
	{
	  // no tracing here. If we were off while receiving, we shouldn't even know that something was there
	  return false;
	}

      m_upstreamDUHandler (this, du_i);
      return true;
    }

    void
    Face::SetMetric (uint16_t metric)
    {
      NS_LOG_FUNCTION (metric);
      m_metric = metric;
    }

    uint16_t
    Face::GetMetric (void) const
    {
      return m_metric;
    }

    void
    Face::SetFlags (uint32_t flags)
    {
      m_flags = flags;
    }

    void
    Face::SetAddress (Address addr)
    {
      m_addr = addr;
    }

    Address
    Face::GetAddress () const
    {
      return m_addr;
    }

    bool
    Face::isAppFace() const
    {
      return (GetFlags () == Face::APPLICATION);
    }

    bool
    Face::operator== (const Face &face) const
    {
      NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                     "NN_Faces of different nodes should not be compared to each other: " << *this << " == " << face);

      return (m_id == face.m_id);
    }

    bool
    Face::operator< (const Face &face) const
    {
      NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                     "NN_Faces of different nodes should not be compared to each other: " << *this << " == " << face);

      return (m_id < face.m_id);
    }

    std::ostream&
    Face::Print (std::ostream &os) const
    {
      os << "id=" << GetId ();
      return os;
    }

    std::ostream&
    operator<< (std::ostream& os, const Face &face)
    {
      face.Print (os);
      return os;
    }

  } // namespace nnn
} // namespace ns3
