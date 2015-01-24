/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko
 *
 *  nnn-consumer-cbr.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-consumer-cbr.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-consumer-cbr.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/double.h>
#include <ns3-dev/ns3/integer.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>
#include <ns3-dev/ns3/uinteger.h>

#include <ns3-dev/ns3/ndn-name.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>

#include "nnn-consumer-cbr.h"

NS_LOG_COMPONENT_DEFINE ("nnn.ConsumerCbr");

namespace ns3 {
  namespace nnn {

    NS_OBJECT_ENSURE_REGISTERED (ConsumerCbr);

    TypeId
    ConsumerCbr::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::ConsumerCbr")
	.SetGroupName ("Nnn")
	.SetParent<Consumer> ()
	.AddConstructor<ConsumerCbr> ()

	.AddAttribute ("Frequency", "Frequency of interest packets",
		       StringValue ("1.0"),
		       MakeDoubleAccessor (&ConsumerCbr::m_frequency),
		       MakeDoubleChecker<double> ())

	.AddAttribute ("Randomize", "Type of send time randomization: none (default), uniform, exponential",
		       StringValue ("none"),
		       MakeStringAccessor (&ConsumerCbr::SetRandomize, &ConsumerCbr::GetRandomize),
		       MakeStringChecker ())

	.AddAttribute ("MaxSeq",
		       "Maximum sequence number to request",
		       IntegerValue (std::numeric_limits<uint32_t>::max ()),
		       MakeIntegerAccessor (&ConsumerCbr::m_seqMax),
		       MakeIntegerChecker<uint32_t> ())
	;

      return tid;
    }

    ConsumerCbr::ConsumerCbr ()
    : m_frequency (1.0)
    , m_firstTime (true)
    , m_random (0)
    {
      NS_LOG_FUNCTION_NOARGS ();
      m_seqMax = std::numeric_limits<uint32_t>::max ();
    }

    ConsumerCbr::~ConsumerCbr ()
    {
      if (m_random)
	delete m_random;
    }

    void
    ConsumerCbr::ScheduleNextPacket ()
    {
      // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
      // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

      if (m_firstTime)
	{
	  m_sendEvent = Simulator::Schedule (Seconds (0.0),
					     &Consumer::SendPacket, this);
	  m_firstTime = false;
	}
      else if (!m_sendEvent.IsRunning ())
	m_sendEvent = Simulator::Schedule (
	    (m_random == 0) ?
		Seconds(1.0 / m_frequency)
		:
		Seconds(m_random->GetValue ()),
		&Consumer::SendPacket, this);
    }

    void
    ConsumerCbr::SetRandomize (const std::string &value)
    {
      if (m_random)
	delete m_random;

      if (value == "uniform")
	{
	  m_random = new UniformVariable (0.0, 2 * 1.0 / m_frequency);
	}
      else if (value == "exponential")
	{
	  m_random = new ExponentialVariable (1.0 / m_frequency, 50 * 1.0 / m_frequency);
	}
      else
	m_random = 0;

      m_randomType = value;
    }

    std::string
    ConsumerCbr::GetRandomize () const
    {
      return m_randomType;
    }


    ///////////////////////////////////////////////////
    //          Process incoming packets             //
    ///////////////////////////////////////////////////

    // void
    // Consumer::OnData (const Ptr<const Data> &contentObject,
    //                                const Ptr<const Packet> &payload)
    // {
    //   Consumer::OnData (contentObject, payload); // tracing inside
    // }

    // void
    // Consumer::OnNack (const Ptr<const Interest> &interest)
    // {
    //   Consumer::OnNack (interest); // tracing inside
    // }

  } // namespace nnn
} // namespace ns3
