/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko and Alexander Afanasyev
 *
 *  nnn-producer.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-producer.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-producer.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>
#include <ns3-dev/ns3/uinteger.h>

#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-name.h>
#include <ns3-dev/ns3/ndn-header-helper.h>
#include <ns3-dev/ns3/ndn-wire.h>
#include <ns3-dev/ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h>

#include <boost/ref.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace ll = boost::lambda;

#include "nnn-producer.h"
#include "../model/nnn-app-face.h"
#include "../model/fib/nnn-fib.h"
#include "../model/fw/nnn-forwarding-strategy.h"

NS_LOG_COMPONENT_DEFINE ("nnn.Producer");

namespace ns3 {
  namespace nnn {

    NS_OBJECT_ENSURE_REGISTERED (Producer);

    TypeId
    Producer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::Producer")
	.SetGroupName ("Nnn")
	.SetParent<App> ()
	.AddConstructor<Producer> ()
	.AddAttribute ("Prefix","Prefix, for which producer has the data",
		       StringValue ("/"),
		       MakeNameAccessor (&Producer::m_prefix),
		       ndn::MakeNameChecker ())
	.AddAttribute ("Postfix", "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
		       StringValue ("/"),
		       MakeNameAccessor (&Producer::m_postfix),
		       ndn::MakeNameChecker ())
	.AddAttribute ("PayloadSize", "Virtual payload size for Content packets",
		       UintegerValue (1024),
		       MakeUintegerAccessor (&Producer::m_virtualPayloadSize),
		       MakeUintegerChecker<uint32_t> ())
	.AddAttribute ("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
		       TimeValue (Seconds (0)),
		       MakeTimeAccessor (&Producer::m_freshness),
		       MakeTimeChecker ())
	.AddAttribute ("Signature", "Fake signature, 0 valid signature (default), other values application-specific",
		       UintegerValue (0),
		       MakeUintegerAccessor (&Producer::m_signature),
		       MakeUintegerChecker<uint32_t> ())
	.AddAttribute ("KeyLocator", "Name to be used for key locator.  If root, then key locator is not used",
		       ndn::NameValue (),
		       MakeNameAccessor (&Producer::m_keyLocator),
		       ndn::MakeNameChecker ())
	.AddAttribute ("UseDU", "Node using 3N DU PDUs",
		       BooleanValue (false),
		       MakeBooleanAccessor(&Producer::m_useDU),
		       MakeBooleanChecker ())
	;
      return tid;
    }

    Producer::Producer ()
    {
      // NS_LOG_FUNCTION_NOARGS ();
    }

    // inherited from Application base class.
    void
    Producer::StartApplication ()
    {
      NS_LOG_FUNCTION_NOARGS ();
      NS_ASSERT (GetNode ()->GetObject<Fib> () != 0);

      App::StartApplication ();

      NS_LOG_DEBUG ("NodeID: " << GetNode ()->GetId ());

      Ptr<Fib> fib = GetNode ()->GetObject<Fib> ();

      Ptr<fib::Entry> fibEntry = fib->Add (m_prefix, m_face, 0);

      fibEntry->UpdateStatus (m_face, fib::FaceMetric::NDN_FIB_GREEN);

      // // make face green, so it will be used primarily
      // StaticCast<fib::FibImpl> (fib)->modify (fibEntry,
      //                                        ll::bind (&fib::Entry::UpdateStatus,
      //                                                  ll::_1, m_face, fib::FaceMetric::NDN_FIB_GREEN));
    }

    void
    Producer::StopApplication ()
    {
      NS_LOG_FUNCTION_NOARGS ();
      NS_ASSERT (GetNode ()->GetObject<Fib> () != 0);

      App::StopApplication ();
    }

    void
    Producer::OnInterest (Ptr<const ndn::Interest> interest)
    {
      App::OnInterest (interest); // tracing inside

      NS_LOG_FUNCTION (this << interest);

      if (!m_active) return;

      Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (m_virtualPayloadSize));
      Ptr<ndn::Name> dataName = Create<ndn::Name> (interest->GetName ());
      dataName->append (m_postfix);
      data->SetName (dataName);
      data->SetFreshness (m_freshness);
      data->SetTimestamp (Simulator::Now());

      data->SetSignature (m_signature);
      if (m_keyLocator.size () > 0)
	{
	  data->SetKeyLocator (Create<ndn::Name> (m_keyLocator));
	}

      NS_LOG_INFO ("node("<< GetNode()->GetId() <<") responding with Data: " << data->GetName ());

      // Echo back FwHopCountTag if exists
      ndn::FwHopCountTag hopCountTag;
      if (interest->GetPayload ()->PeekPacketTag (hopCountTag))
	{
	  data->GetPayload ()->AddPacketTag (hopCountTag);
	}
      // Will have to be adapted to 3N
      //m_face->ReceiveData (data);
      m_transmittedDatas (data, this, m_face);
    }

    Ptr<Packet>
    Producer::CreateReturnData (Ptr<ndn::Interest> interest)
    {
      Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (m_virtualPayloadSize));
      Ptr<ndn::Name> dataName = Create<ndn::Name> (interest->GetName ());
      dataName->append (m_postfix);
      data->SetName (dataName);
      data->SetFreshness (m_freshness);
      data->SetTimestamp (Simulator::Now());

      data->SetSignature (m_signature);
      if (m_keyLocator.size () > 0)
	{
	  data->SetKeyLocator (Create<ndn::Name> (m_keyLocator));
	}

      NS_LOG_INFO ("node("<< GetNode()->GetId() <<") responding with Data: " << data->GetName ());

      // Echo back FwHopCountTag if exists
      ndn::FwHopCountTag hopCountTag;
      if (interest->GetPayload ()->PeekPacketTag (hopCountTag))
	{
	  data->GetPayload ()->AddPacketTag (hopCountTag);
	}

      m_transmittedDatas (data, this, m_face);
      return ndn::Wire::FromData(data);
    }

    void
    Producer::OnNULLp (Ptr<const NULLp> nullpObject)
    {
      if (!m_active) return;

      App::OnNULLp(nullpObject);

      NS_LOG_FUNCTION (this << nullpObject);

      Ptr<Packet> packet = nullpObject->GetPayload ()->Copy ();
      uint16_t pdutype = nullpObject->GetPDUPayloadType ();

      if (pdutype == NDN_NNN)
	{
	  try
	  {
	      ndn::HeaderHelper::Type type = ndn::HeaderHelper::GetNdnHeaderType (packet);
	      Ptr<ndn::Interest> interest = 0;
	      switch (type)
	      {
		case ndn::HeaderHelper::INTEREST_NDNSIM:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_NDNSIM);
		  break;
		case ndn::HeaderHelper::INTEREST_CCNB:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_CCNB);
		  break;
	      }

	      if (interest != 0)
		{
		  App::OnInterest (interest);

		  Ptr<Packet> retPkt = CreateReturnData(interest);

		  if (m_useDU)
		    {
		      // We don't have all the information for a DU, so send a SO
		      Ptr<SO> so_o = Create<SO> ();
		      so_o->SetPDUPayloadType(pdutype);
		      Ptr<NNNAddress> tmp = Create<NNNAddress> (GetNode ()->GetObject<ForwardingStrategy> ()->GetNode3NName ());
		      so_o->SetName(tmp);
		      so_o->SetLifetime(m_3n_lifetime);

		      m_face->ReceiveSO(so_o);
		      m_transmittedSOs (so_o, this, m_face);
		    }
		  else
		    {
		      Ptr<NULLp> nullp_o = Create<NULLp> ();

		      nullp_o->SetPDUPayloadType (pdutype);
		      nullp_o->SetPayload (retPkt);
		      nullp_o->SetLifetime(m_3n_lifetime);

		      m_face->ReceiveNULLp(nullp_o);
		      m_transmittedNULLps (nullp_o, this, m_face);
		    }
		}

	      // exception will be thrown if packet is not recognized
	  }
	  catch (ndn::UnknownHeaderException)
	  {
	      NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
	  }
	}
    }

    void
    Producer::OnSO (Ptr<const SO> soObject)
    {
      if (!m_active) return;

      App::OnSO(soObject);

      NS_LOG_FUNCTION (this << soObject);

      Ptr<Packet> packet = soObject->GetPayload ()->Copy ();
      uint16_t pdutype = soObject->GetPDUPayloadType ();

      if (pdutype == NDN_NNN)
	{
	  try
	  {
	      ndn::HeaderHelper::Type type = ndn::HeaderHelper::GetNdnHeaderType (packet);
	      Ptr<ndn::Interest> interest = 0;
	      switch (type)
	      {
		case ndn::HeaderHelper::INTEREST_NDNSIM:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_NDNSIM);
		  break;
		case ndn::HeaderHelper::INTEREST_CCNB:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_CCNB);
		  break;
	      }

	      if (interest != 0)
		{
		  App::OnInterest (interest);

		  Ptr<Packet> retPkt = CreateReturnData(interest);

		  if (m_useDU)
		    {
		      // We can use DU packets now
		      Ptr<DU> du_o = Create<DU> ();
		      du_o->SetPDUPayloadType(pdutype);
		      Ptr<NNNAddress> tmp = Create<NNNAddress> (GetNode ()->GetObject<ForwardingStrategy> ()->GetNode3NName ());
		      du_o->SetSrcName(tmp);
		      Ptr<NNNAddress> tmp2 = Create<NNNAddress> (soObject->GetName());
		      du_o->SetDstName(tmp2);
		      du_o->SetLifetime(m_3n_lifetime);

		      m_face->ReceiveDU(du_o);
		      m_transmittedDUs (du_o, this, m_face);
		    }
		  else
		    {
		      Ptr<DO> do_o = Create<DO> ();

		      do_o->SetName (soObject->GetName ());
		      do_o->SetPDUPayloadType (pdutype);
		      do_o->SetPayload (retPkt);
		      do_o->SetLifetime(m_3n_lifetime);

		      m_face->ReceiveDO(do_o);
		      m_transmittedDOs (do_o, this, m_face);
		    }
		}

	      // exception will be thrown if packet is not recognized
	  }
	  catch (ndn::UnknownHeaderException)
	  {
	      NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
	  }
	}
    }

    void
    Producer::OnDU (Ptr<const DU> duObject)
    {
      if (!m_active) return;

      App::OnDU(duObject);

      NS_LOG_FUNCTION (this << duObject);

      Ptr<Packet> packet = duObject->GetPayload ()->Copy ();
      uint16_t pdutype = duObject->GetPDUPayloadType ();

      if (pdutype == NDN_NNN)
	{
	  try
	  {
	      ndn::HeaderHelper::Type type = ndn::HeaderHelper::GetNdnHeaderType (packet);
	      Ptr<ndn::Interest> interest = 0;
	      switch (type)
	      {
		case ndn::HeaderHelper::INTEREST_NDNSIM:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_NDNSIM);
		  break;
		case ndn::HeaderHelper::INTEREST_CCNB:
		  interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_CCNB);
		  break;
	      }

	      if (interest != 0)
		{
		  App::OnInterest (interest);

		  Ptr<Packet> retPkt = CreateReturnData(interest);

		  Ptr<DU> du_o = Create<DU> ();

		  du_o->SetSrcName (duObject->GetDstName ());
		  du_o->SetDstName (duObject->GetSrcName ());
		  du_o->SetPDUPayloadType (pdutype);
		  du_o->SetPayload (retPkt);
		  du_o->SetLifetime (m_3n_lifetime);

		  m_face->ReceiveDU(du_o);
		  m_transmittedDUs (du_o, this, m_face);
		}

	      // exception will be thrown if packet is not recognized
	  }
	  catch (ndn::UnknownHeaderException)
	  {
	      NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
	  }
	}


    }
  } // namespace nnn
} // namespace ns3
