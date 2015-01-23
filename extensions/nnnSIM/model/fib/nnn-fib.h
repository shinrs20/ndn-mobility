/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-fib.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-fib.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-fib.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_FIB_H_
#define	_NNN_FIB_H_

#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#include "nnn-fib-entry.h"

namespace ns3 {
  namespace nnn {

    class Interest;
    typedef Interest InterestHeader;

    /**
     * @ingroup nnn
     * @defgroup nnn-fib FIB
     */

    /**
     * @ingroup nnn-fib
     * @brief Class implementing FIB functionality
     */
    class Fib : public Object
    {
    public:
      /**
       * \brief Interface ID
       *
       * \return interface ID
       */
      static TypeId GetTypeId ();
      /**
       * @brief Default constructor
       */
      Fib () {}

      /**
       * @brief Virtual destructor
       */
      virtual ~Fib () { };

      /**
       * \brief Perform longest prefix match
       *
       * \todo Implement exclude filters
       *
       * \param interest Interest packet header
       * \returns If entry found a valid iterator (Ptr<fib::Entry>) will be returned, otherwise End () (==0)
       */
      virtual Ptr<fib::Entry>
      LongestPrefixMatch (const ndn::Interest &interest) = 0;

      /**
       * @brief Get FIB entry for the prefix (exact match)
       *
       * @param prefix Name for FIB entry
       * @returns If entry is found, a valid iterator (Ptr<fib::Entry>) will be returned. Otherwise End () (==0)
       */
      virtual Ptr<fib::Entry>
      Find (const ndn::Name &prefix) = 0;

      /**
       * \brief Add or update FIB entry
       *
       * If the entry exists, metric will be updated. Otherwise, new entry will be created
       *
       * @param name	Prefix
       * @param face	Forwarding face
       * @param metric	Routing metric
       */
      virtual Ptr<fib::Entry>
      Add (const ndn::Name &prefix, Ptr<Face> face, int32_t metric) = 0;

      /**
       * \brief Add or update FIB entry using smart pointer to prefix
       *
       * If the entry exists, metric will be updated. Otherwise, new entry will be created
       *
       * @param name	Smart pointer to prefix
       * @param face	Forwarding face
       * @param metric	Routing metric
       */
      virtual Ptr<fib::Entry>
      Add (const Ptr<const ndn::Name> &prefix, Ptr<Face> face, int32_t metric) = 0;

      /**
       * @brief Remove FIB entry
       *
       * ! ATTENTION ! Use with caution.  All PIT entries referencing the corresponding FIB entry will become invalid.
       * So, simulation may crash.
       *
       * @param name	Smart pointer to prefix
       */
      virtual void
      Remove (const Ptr<const ndn::Name> &prefix) = 0;

      /**
       * @brief Invalidate all FIB entries
       */
      virtual void
      InvalidateAll () = 0;

      /**
       * @brief Remove all references to a face from FIB.  If for some enty that face was the only element,
       * this FIB entry will be removed.
       */
      virtual void
      RemoveFromAll (Ptr<Face> face) = 0;

      /**
       * @brief Print out entries in FIB
       */
      virtual void
      Print (std::ostream &os) const = 0;

      /**
       * @brief Get number of entries in FIB
       */
      virtual uint32_t
      GetSize () const = 0;

      /**
       * @brief Return first element of FIB (no order guaranteed)
       */
      virtual Ptr<const fib::Entry>
      Begin () const = 0;

      /**
       * @brief Return first element of FIB (no order guaranteed)
       */
      virtual Ptr<fib::Entry>
      Begin () = 0;

      /**
       * @brief Return item next after last (no order guaranteed)
       */
      virtual Ptr<const fib::Entry>
      End () const = 0;

      /**
       * @brief Return item next after last (no order guaranteed)
       */
      virtual Ptr<fib::Entry>
      End () = 0;

      /**
       * @brief Advance the iterator
       */
      virtual Ptr<const fib::Entry>
      Next (Ptr<const fib::Entry>) const = 0;

      /**
       * @brief Advance the iterator
       */
      virtual Ptr<fib::Entry>
      Next (Ptr<fib::Entry>) = 0;

      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////

      /**
       * @brief Static call to cheat python bindings
       */
      static inline Ptr<Fib>
      GetFib (Ptr<Object> node);

      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////

    private:
      Fib (const Fib&) {} ; ///< \brief copy constructor is disabled
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    std::ostream& operator<< (std::ostream& os, const Fib &fib);

    Ptr<Fib>
    Fib::GetFib (Ptr<Object> node)
    {
      return node->GetObject<Fib> ();
    }

  } // namespace ndn
} // namespace ns3

#endif // _NNN_FIB_H_
