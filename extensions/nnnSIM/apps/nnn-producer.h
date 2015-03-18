/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko and Alexander Afanasyev
 *
 *  nnn-producer.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-producer.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-producer.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NNN_PRODUCER_H
#define NNN_PRODUCER_H

#include <ns3-dev/ns3/ptr.h>

#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-interest.h>

#include "nnn-app.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * @ingroup nnn-apps
     * @brief A simple Interest-sink application
     *
     * A simple Interest-sink application, which replying every incoming Interest with
     * Data packet with a specified size and name same as in Interest.
     */
    class Producer : public App
    {
    public:
      static TypeId
      GetTypeId (void);

      Producer ();

      Ptr<Packet>
      CreateReturnData (Ptr<ndn::Interest> interest);

      // inherited from NnnApp
      void OnInterest (Ptr<const ndn::Interest> interest);

      // Essentially to de-encapsulate DU
      void OnNULLp (Ptr<const NULLp> nullpObject);

      // Essentially to de-encapsulate SO
      void OnSO (Ptr<const SO> soObject);

      // Essentially to de-encapsulate DU
      void OnDU (Ptr<const DU> duObject);

    protected:
      // inherited from Application base class.
      virtual void
      StartApplication ();    // Called at time specified by Start

      virtual void
      StopApplication ();     // Called at time specified by Stop

    private:
      ndn::Name m_prefix;
      ndn::Name m_postfix;
      uint32_t m_virtualPayloadSize;
      Time m_freshness;

      uint32_t m_signature;
      ndn::Name m_keyLocator;
      bool m_useDU;
    };

  } // namespace nnn
} // namespace ns3

#endif // NNN_PRODUCER_H
