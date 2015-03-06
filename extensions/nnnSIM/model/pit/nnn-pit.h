/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-pit.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pit.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pit.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_PIT_H_
#define	_NNN_PIT_H_

#include <ns3-dev/ns3/event-id.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/object.h>

#include "nnn-pit-entry.h"

namespace ns3 {

  namespace ndn {
    class Data;
    class Interest;
  }
  namespace nnn {

    /**
     * * @ingroup nnn
     * * @defgroup nnn-pit PIT
     */

    /**
     * @ingroup nnn-pit
     * @brief Namespace for PIT operations
     */
    namespace pit {
    }

    class L3Protocol;
    class Face;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    /**
     * @ingroup nnn-pit
     * @brief Class implementing Pending Interests Table
     */
    class Pit : public Object
    {
    public:
      /**
       * \brief Interface ID
       *
       * \return interface ID
       */
      static TypeId GetTypeId ();

      /**
       * \brief PIT constructor
       */
      Pit ();

      /**
       * \brief Destructor
       */
      virtual ~Pit ();

      /**
       * \brief Find corresponding PIT entry for the given content name
       *
       * Not that this call should be repeated enough times until it return 0.
       * This way all records with shorter or equal prefix as in content object will be found
       * and satisfied.
       *
       * \param prefix Prefix for which to lookup the entry
       * \returns smart pointer to PIT entry. If record not found,
       *          returns 0
       */
      virtual Ptr<pit::Entry>
      Lookup (const ndn::Data &header) = 0;

      /**
       * \brief Find a PIT entry for the given content interest
       * \param header parsed interest header
       * \returns iterator to Pit entry. If record not found,
       *          return end() iterator
       */
      virtual Ptr<pit::Entry>
      Lookup (const ndn::Interest &header) = 0;

      /**
       * @brief Get PIT entry for the prefix (exact match)
       *
       * @param prefix Name for PIT entry
       * @returns If entry is found, a valid iterator (Ptr<pit::Entry>) will be returned. Otherwise End () (==0)
       */
      virtual Ptr<pit::Entry>
      Find (const ndn::Name &prefix) = 0;

      /**
       * @brief Creates a PIT entry for the given interest
       * @param header parsed interest header
       * @returns iterator to Pit entry. If record could not be created (e.g., limit reached),
       *          return end() iterator
       *
       * Note. This call assumes that the entry does not exist (i.e., there was a Lookup call before)
       */
      virtual Ptr<pit::Entry>
      Create (Ptr<const ndn::Interest> header) = 0;

      /**
       * @brief Mark PIT entry deleted
       * @param entry PIT entry
       *
       * Effectively, this method removes all incoming/outgoing faces and set
       * lifetime +m_PitEntryDefaultLifetime from Now ()
       */
      virtual void
      MarkErased (Ptr<pit::Entry> entry) = 0;

      /**
       * @brief Print out PIT contents for debugging purposes
       *
       * Note that there is no definite order in which entries are printed out
       */
      virtual void
      Print (std::ostream &os) const = 0;

      /**
       * @brief Get number of entries in PIT
       */
      virtual uint32_t
      GetSize () const = 0;

      /**
       * @brief Return first element of FIB (no order guaranteed)
       */
      virtual Ptr<pit::Entry>
      Begin () = 0;

      /**
       * @brief Return item next after last (no order guaranteed)
       */
      virtual Ptr<pit::Entry>
      End () = 0;

      /**
       * @brief Advance the iterator
       */
      virtual Ptr<pit::Entry>
      Next (Ptr<pit::Entry>) = 0;

      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////

      /**
       * @brief Static call to cheat python bindings
       */
      static inline Ptr<Pit>
      GetPit (Ptr<Object> node);

      /**
       * @brief Get maximum PIT entry lifetime
       */
      inline const Time&
      GetMaxPitEntryLifetime () const;

      /**
       * @brief Set maximum PIT entry lifetime
       */
      inline void
      SetMaxPitEntryLifetime (const Time &maxLifetime);

    protected:
      // configuration variables. Check implementation of GetTypeId for more details
      Time m_PitEntryPruningTimout;

      Time m_maxPitEntryLifetime;
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    inline std::ostream&
    operator<< (std::ostream& os, const Pit &pit)
    {
      pit.Print (os);
      return os;
    }

    inline Ptr<Pit>
    Pit::GetPit (Ptr<Object> node)
    {
      return node->GetObject<Pit> ();
    }

    inline const Time&
    Pit::GetMaxPitEntryLifetime () const
    {
      return m_maxPitEntryLifetime;
    }

    inline void
    Pit::SetMaxPitEntryLifetime (const Time &maxLifetime)
    {
      m_maxPitEntryLifetime = maxLifetime;
    }
  } // namespace nnn
} // namespace ns3
#endif	/* NNN_PIT_H */
