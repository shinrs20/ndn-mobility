/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-app.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-app.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-app.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_APP_H
#define NNN_APP_H

#include <ns3-dev/ns3/application.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/traced-callback.h>
#include <ns3-dev/ns3/ptr.h>

#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-interest.h>

namespace ns3 {

  class Packet;

  namespace nnn {

    class SO;
    class DO;
    class DU;

    class Face;

    /**
     * \ingroup nnn
     * \defgroup nnn-apps NDN applications
     */
    /**
     * @ingroup nnn-apps
     * @brief Base class that all NDN applications should be derived from.
     *
     * The class implements virtual calls onInterest, onNack, and onData
     */
    class App: public Application
    {
    public:
      static TypeId GetTypeId ();

      /**
       * @brief Default constructor
       */
      App ();
      virtual ~App ();

      /**
       * @brief Get application ID (ID of applications face)
       */
      uint32_t
      GetId () const;

      /**
       * @brief Method that will be called every time new Interest arrives
       * @param interest Interest header
       * @param packet   "Payload" of the interests packet. The actual payload should be zero, but packet itself
       *                 may be useful to get packet tags
       */
      virtual void
      OnInterest (Ptr<const ndn::Interest> interest);

      /**
       * @brief Method that will be called every time new NACK arrives
       * @param interest Interest header
       */
      virtual void
      OnNack (Ptr<const ndn::Interest> interest);

      /**
       * @brief Method that will be called every time new Data arrives
       * @param contentObject Data header
       * @param payload payload (potentially virtual) of the Data packet (may include packet tags of original packet)
       */
      virtual void
      OnData (Ptr<const ndn::Data> contentObject);

      virtual void
      OnSO (Ptr<const SO> soObject);

      virtual void
      OnDO (Ptr<const DO> doObject);

      virtual void
      OnDU (Ptr<const DU> duObject);

    protected:
      /**
       * @brief Do cleanup when application is destroyed
       */
      virtual void
      DoDispose ();

      // inherited from Application base class. Originally they were private
      virtual void
      StartApplication ();    ///< @brief Called at time specified by Start

      virtual void
      StopApplication ();     ///< @brief Called at time specified by Stop

    protected:
      bool m_active;  ///< @brief Flag to indicate that application is active (set by StartApplication and StopApplication)
      Ptr<Face> m_face;   ///< @brief automatically created application face through which application communicates

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<App>, Ptr<Face> > m_receivedInterests; ///< @brief App-level trace of received Interests

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<App>, Ptr<Face> > m_receivedNacks; ///< @brief App-level trace of received NACKs

      TracedCallback<Ptr<const ndn::Data>,
      Ptr<App>, Ptr<Face> > m_receivedDatas; ///< @brief App-level trace of received Data

      TracedCallback<Ptr<const SO>,
      Ptr<App>, Ptr<Face> > m_receivedSO; ///< @brief App-level trace of received SO

      TracedCallback<Ptr<const DO>,
      Ptr<App>, Ptr<Face> > m_receivedDO; ///< @brief App-level trace of received DO

      TracedCallback<Ptr<const DU>,
      Ptr<App>, Ptr<Face> > m_receivedDU; ///< @brief App-level trace of received DU

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<App>, Ptr<Face> > m_transmittedInterests; ///< @brief App-level trace of transmitted Interests

      TracedCallback<Ptr<const ndn::Data>,
      Ptr<App>, Ptr<Face> > m_transmittedDatas; ///< @brief App-level trace of transmitted Data
    };

  } // namespace nnn
} // namespace ns3

#endif // NNN_APP_H
