/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-fib-impl.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-fib-impl.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-fib-impl.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_FIB_IMPL_H_
#define	_NNN_FIB_IMPL_H_

#include "nnn-fib.h"

#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/trie-with-policy.h>
#include <ns3-dev/ns3/ndnSIM/utils/trie/counting-policy.h>

namespace ns3
{
  namespace nnn
  {
    namespace fib
    {
      /**
       * @ingroup ndn-fib
       * @brief FIB entry implementation with with additional references to the base container
       */
      class EntryImpl : public Entry
      {
      public:
	typedef ndn::ndnSIM::trie_with_policy<
	    ns3::ndn::Name,
	    ndn::ndnSIM::smart_pointer_payload_traits<EntryImpl>,
	    ndn::ndnSIM::counting_policy_traits
	    > trie;

	EntryImpl (Ptr<Fib> fib, const Ptr<const ndn::Name> &prefix)
	: Entry (fib, prefix)
	, item_ (0)
	{
	}

	void
	SetTrie (trie::iterator item)
	{
	  item_ = item;
	}

	trie::iterator to_iterator () { return item_; }
	trie::const_iterator to_iterator () const { return item_; }

      private:
	trie::iterator item_;
      };

      /**
       * @ingroup ndn-fib
       * \brief Class implementing FIB functionality
       */
      class FibImpl : public Fib,
      protected ndn::ndnSIM::trie_with_policy<ndn::Name,
      ndn::ndnSIM::smart_pointer_payload_traits< EntryImpl >,
      ndn::ndnSIM::counting_policy_traits >
      {
      public:
	typedef ndn::ndnSIM::trie_with_policy<ndn::Name,
	    ndn::ndnSIM::smart_pointer_payload_traits<EntryImpl>,
	    ndn::ndnSIM::counting_policy_traits > super;

	/**
	 * \brief Interface ID
	 *
	 * \return interface ID
	 */
	static TypeId GetTypeId ();

	/**
	 * \brief Constructor
	 */
	FibImpl ();

	virtual Ptr<Entry>
	LongestPrefixMatch (const ndn::Interest &interest);

	virtual Ptr<fib::Entry>
	Find (const ndn::Name &prefix);

	virtual Ptr<Entry>
	Add (const ndn::Name &prefix, Ptr<Face> face, int32_t metric);

	virtual Ptr<Entry>
	Add (const Ptr<const ndn::Name> &prefix, Ptr<Face> face, int32_t metric);

	virtual void
	Remove (const Ptr<const ndn::Name> &prefix);

	virtual void
	InvalidateAll ();

	virtual void
	RemoveFromAll (Ptr<Face> face);

	virtual void
	Print (std::ostream &os) const;

	virtual uint32_t
	GetSize () const;

	virtual Ptr<const Entry>
	Begin () const;

	virtual Ptr<Entry>
	Begin ();

	virtual Ptr<const Entry>
	End () const;

	virtual Ptr<Entry>
	End ();

	virtual Ptr<const Entry>
	Next (Ptr<const Entry> item) const;

	virtual Ptr<Entry>
	Next (Ptr<Entry> item);

      protected:
	// inherited from Object class
	virtual void NotifyNewAggregate (); ///< @brief Notify when object is aggregated
	virtual void DoDispose (); ///< @brief Perform cleanup

      private:
	/**
	 * @brief Remove reference to a face from the entry. If entry had only this face, the whole
	 * entry will be removed
	 */
	void
	RemoveFace (super::parent_trie &item, Ptr<Face> face);
      };

    } // namespace fib
  } // namespace nnn
} // namespace ns3

#endif	/* _NNN_FIB_IMPL_H_ */
