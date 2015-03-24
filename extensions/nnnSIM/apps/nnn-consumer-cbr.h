/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
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

#ifndef NNN_CONSUMER_CBR_H
#define NNN_CONSUMER_CBR_H

#include "nnn-consumer.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * @ingroup nnn-apps
     * @brief Nnn application for sending out Interest packets at a "constant" rate (Poisson process)
     */
    class ConsumerCbr: public Consumer
    {
    public:
      static TypeId GetTypeId ();

      /**
       * \brief Default constructor
       * Sets up randomizer function and packet sequence number
       */
      ConsumerCbr ();
      virtual ~ConsumerCbr ();

    protected:
      /**
       * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN protocol
       */
      virtual void
      ScheduleNextPacket ();

      /**
       * @brief Set type of frequency randomization
       * @param value Either 'none', 'uniform', or 'exponential'
       */
      void
      SetRandomize (const std::string &value);

      /**
       * @brief Get type of frequency randomization
       * @returns either 'none', 'uniform', or 'exponential'
       */
      std::string
      GetRandomize () const;

    protected:
      double              m_frequency; // Frequency of interest packets (in hertz)
      bool                m_firstTime;
      RandomVariable      *m_random;
      std::string         m_randomType;
    };
  } // namespace nnn
} // namespace ns3

#endif
