/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnst.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnst.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst.cc.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/core.hpp>
#include <boost/ref.hpp>

namespace ll = boost::lambda;

#include "nnn-nnst.h"
#include "nnn-nnst-entry.h"

NS_LOG_COMPONENT_DEFINE ("nnn.nnst");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (NNST);

    TypeId
    NNST::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::NNST") // cheating ns3 object system
    			.SetParent<Object> ()
    		    .SetGroupName ("Nnn")
    		    .AddConstructor<NNST> ();
      return tid;
    }

    NNST::NNST() {
    }

    NNST::~NNST() {
    }

    Ptr<nnst::Entry>
    NNST::ClosestSector (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      super::iterator item = super::longest_prefix_match (prefix);

      if (item == super::end ())
	return 0;
      else
	return item->payload ();
    }

    Ptr<const NNNAddress>
    NNST::ClosestSectorNameInfo (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      Ptr<nnst::Entry> tmp = ClosestSector (prefix);
      if (tmp != 0)
	return tmp->GetAddressPtr();
      else
	return Create<const NNNAddress> ();
    }

    Ptr<const NNNAddress>
    NNST::ClosestSectorNameInfo (Ptr<const NNNAddress> prefix)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return ClosestSectorNameInfo(*prefix);
    }

    std::pair<Ptr<Face>, Address>
    NNST::ClosestSectorFaceInfo (const NNNAddress &prefix, uint32_t skip)
    {
        NS_LOG_FUNCTION (this << prefix);
        // Find the closest entry to prefix.
        Ptr<nnst::Entry> tmp = ClosestSector(prefix);

        return tmp->FindBestCandidateFaceInfo(skip);
    }

    std::pair<Ptr<Face>, Address>
    NNST::ClosestSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip)
    {
        NS_LOG_FUNCTION (this << *prefix);
        return ClosestSectorFaceInfo(*prefix, skip);
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopNameInfo (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      Ptr<nnst::Entry> curr;
      std::vector<Ptr<const NNNAddress> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1)
	    {
	      ret.push_back(curr->GetAddressPtr());
	    }
	}

      return ret;
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopNameInfo (Ptr<const NNNAddress> prefix)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopNameInfo (*prefix);
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopFaceInfo (const NNNAddress &prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << prefix);

      Ptr<nnst::Entry> curr;
      std::vector<std::pair<Ptr<Face>, Address> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1)
	    {
	      ret.push_back(curr->FindBestCandidateFaceInfo(skip));
	    }
	}

      return ret;
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopFaceInfo(*prefix, skip);
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopSubSectorNameInfo (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);

      Ptr<nnst::Entry> curr;
      std::vector<Ptr<const NNNAddress> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1 && prefix.isParentSector(curr->GetAddress()))
	    {
	      ret.push_back(curr->GetAddressPtr());
	    }
	}

      return ret;
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopSubSectorNameInfo (Ptr<const NNNAddress> prefix)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopSubSectorNameInfo (*prefix);
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopSubSectorFaceInfo (const NNNAddress &prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << prefix);

      Ptr<nnst::Entry> curr;
      std::vector<std::pair<Ptr<Face>, Address> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1 && prefix.isParentSector(curr->GetAddress()))
	    {
	      ret.push_back(curr->FindBestCandidateFaceInfo(skip));
	    }
	}

      return ret;
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopSubSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopSubSectorFaceInfo(*prefix, skip);
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopParentSectorNameInfo (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);

      Ptr<nnst::Entry> curr;
      std::vector<Ptr<const NNNAddress> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1 && curr->GetAddress().isParentSector(prefix))
	    {
	      ret.push_back(curr->GetAddressPtr ());
	    }
	}

      return ret;
    }

    std::vector<Ptr<const NNNAddress> >
    NNST::OneHopParentSectorNameInfo (Ptr<const NNNAddress> prefix)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopParentSectorNameInfo(*prefix);
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopParentSectorFaceInfo (const NNNAddress &prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << prefix);

      Ptr<nnst::Entry> curr;
      std::vector<std::pair<Ptr<Face>, Address> > ret;

      for (curr = Begin(); curr != End (); curr = Next(curr))
	{
	  if (curr->GetAddress().distance(prefix) == 1 && curr->GetAddress().isParentSector(prefix))
	    {
	      ret.push_back(curr->FindBestCandidateFaceInfo(skip));
	    }
	}

      return ret;
    }

    std::vector<std::pair<Ptr<Face>, Address> >
    NNST::OneHopParentSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip)
    {
      NS_LOG_FUNCTION (this << *prefix);
      return OneHopParentSectorFaceInfo(*prefix, skip);
    }

    Ptr<nnst::Entry>
    NNST::Find (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      super::iterator item = super::find_exact (prefix);

      if (item == super::end ())
	return 0;
      else
	return item->payload ();
    }

    // To be deleted
    Ptr<nnst::Entry>
    NNST::Add (const NNNAddress &prefix, Ptr<Face> face, int32_t metric)
    {
      return Add (Create<NNNAddress> (prefix), face, metric);
    }

    // This one to be deleted as well
    Ptr<nnst::Entry>
    NNST::Add (const Ptr<const NNNAddress> &name, Ptr<Face> face, int32_t metric)
    {
    }

    Ptr<nnst::Entry>
    NNST::Add (const NNNAddress &name, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric)
    {
      NS_LOG_FUNCTION ("const NNNAddress Add" << name);
      char c;
      Ptr<nnst::Entry> tmp = Add (Create<NNNAddress> (name), face, poa, lease_expire, metric, c);

      Simulator::Schedule(lease_expire, &NNST::cleanExpired, this, tmp);
      return tmp;
    }

    Ptr<nnst::Entry>
    NNST::Add (const Ptr<const NNNAddress> &prefix, std::vector<Ptr<Face> > faces, Address poa, Time lease_expire, int32_t metric)
    {
      NS_LOG_FUNCTION ("Face vector Add" << boost::cref(*prefix));
      Ptr<nnst::Entry> tmp;
      char c;
      for (std::vector<Ptr<Face> >::iterator i = faces.begin(); i != faces.end (); ++i)
	{
	  tmp = Add(prefix, *i, poa, lease_expire, metric, c);
	}

      Simulator::Schedule(lease_expire, &NNST::cleanExpired, this, tmp);
      return tmp;
    }

    Ptr<nnst::Entry>
    NNST::Add (const Ptr<const NNNAddress> &prefix, Ptr<Face> face, std::vector<Address> poas, Time lease_expire, int32_t metric)
    {
      NS_LOG_FUNCTION ("Address vector Add" << boost::cref(*prefix));
      Ptr<nnst::Entry> tmp;
      char c;
      for (std::vector<Address>::iterator i = poas.begin(); i != poas.end (); ++i)
	{
	  tmp = Add(prefix, face, *i, lease_expire, metric, c);
	}

      Simulator::Schedule(lease_expire, &NNST::cleanExpired, this, tmp);
      return tmp;
    }

    Ptr<nnst::Entry>
    NNST::Add (const Ptr<const NNNAddress> &name, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric)
    {
      NS_LOG_FUNCTION ("Unitary Add" << boost::cref(*name));
      char c;
      Ptr<nnst::Entry> tmp = Add(name, face, poa, lease_expire, metric, c);

      Simulator::Schedule(lease_expire, &NNST::cleanExpired, this, tmp);
      return tmp;
    }

    void
    NNST::UpdateStatus(const NNNAddress &prefix, Ptr<Face> face, nnst::FaceMetric::Status status)
    {
      NS_LOG_FUNCTION (this << prefix << boost::cref(*face) << status);

      super::iterator item = super::find_exact (prefix);

      if (item != super::end ())
	super::modify (&(*item), ll::bind (&nnst::Entry::UpdateStatus, ll::_1, face, status));
    }

    void
    NNST::UpdateLeaseTime(const NNNAddress &prefix, Time n_lease)
    {
      NS_LOG_FUNCTION (this << prefix << n_lease);

      super::iterator item = super::find_exact (prefix);

      if (item != super::end ())
	{
	  bool ok = super::modify (&(*item), ll::bind (&nnst::Entry::UpdateLeaseTime, ll::_1, n_lease));

	  if (ok)
	    {
	      Ptr<nnst::Entry> tmp = item->payload ();
	      Simulator::Schedule(n_lease, &NNST::cleanExpired, this, tmp);
	    }
	}
    }

    void
    NNST::AddOrUpdateRoutingMetric(const NNNAddress &prefix, Ptr<Face> face, int32_t metric)
    {
      NS_LOG_FUNCTION (this << prefix << boost::cref(*face) << metric);
      super::iterator item = super::find_exact (prefix);

      if (item != super::end ())
	  super::modify (&(*item), ll::bind (&nnst::Entry::AddOrUpdateRoutingMetric, ll::_1, face, metric));
    }

    void
    NNST::UpdateFaceRtt(const NNNAddress &prefix, Ptr<Face> face, const Time &sample)
    {
      NS_LOG_FUNCTION (this << prefix << boost::cref(*face) << sample);
      super::iterator item = super::find_exact (prefix);

      if (item != super::end ())
	super::modify (&(*item), ll::bind (&nnst::Entry::UpdateFaceRtt, ll::_1, face, sample));
    }


    void
    NNST::InvalidateAll ()
    {
      NS_LOG_FUNCTION (this);

      super::parent_trie::recursive_iterator item (super::getTrie ());
      super::parent_trie::recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  super::modify (&(*item),
	                 ll::bind (&nnst::Entry::Invalidate, ll::_1));
	}
    }

    void
    NNST::Remove (const Ptr<const NNNAddress> &name)
    {
      NS_LOG_FUNCTION (boost::cref(*name));

      super::iterator nnstEntry = super::find_exact (*name);
      if (nnstEntry != super::end ())
	{
	  // notify forwarding strategy about soon be removed FIB entry
	  NS_ASSERT (this->GetObject<ForwardingStrategy> () != 0);
	  this->GetObject<ForwardingStrategy> ()->WillRemoveNNSTEntry (nnstEntry->payload ());

	  super::erase (nnstEntry);
	}
    }

    void
    NNST::RemoveFromAll (Ptr<Face> face)
    {
      NS_LOG_FUNCTION (this << boost::cref(*face));

      Ptr<nnst::Entry> entry = Begin ();
      while (entry != End ())
	{
	  entry->RemoveFace (face);
	  if (entry->m_faces.size () == 0)
	    {
	      Ptr<nnst::Entry> nextEntry = Next (entry);

	      // notify forwarding strategy about soon be removed FIB entry
	      NS_ASSERT (this->GetObject<ForwardingStrategy> () != 0);
	      this->GetObject<ForwardingStrategy> ()->WillRemoveNNSTEntry (entry);

	      super::erase (StaticCast<nnst::Entry> (entry)->to_iterator ());
	      entry = nextEntry;
	    }
	  else
	    {
	      entry = Next (entry);
	    }
	}
    }

    void
    NNST::RemoveFromAll (Address poa)
    {
      NS_LOG_FUNCTION (this << poa);

      Ptr<nnst::Entry> entry = Begin ();
      while (entry != End ())
	{
	  entry->RemovePoA (poa);
	  if (entry->m_faces.size () == 0)
	    {
	      Ptr<nnst::Entry> nextEntry = Next (entry);

	      // notify forwarding strategy about soon be removed NNST entry
	      NS_ASSERT (this->GetObject<ForwardingStrategy> () != 0);
	      this->GetObject<ForwardingStrategy> ()->WillRemoveNNSTEntry (entry);

	      super::erase (entry->to_iterator ());
	      entry = nextEntry;
	    }
	  else
	    {
	      entry = Next (entry);
	    }
	}
    }

    void
    NNST::Print (std::ostream &os) const
    {
      // !!! unordered_set imposes "random" order of item in the same level !!!
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  os << *(item->payload ());
	}
    }

    void
    NNST::PrintByMetric () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  item->payload ()->printByMetric ();
	}
    }

    void
    NNST::PrintByAddress () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  item->payload ()->printByAddress ();
	}
    }

    void
    NNST::PrintByLease () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  item->payload ()->printByLease();
	}
    }

    void
    NNST::PrintByFace () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;

	  item->payload ()->printByFace();
	}
    }

    uint32_t
    NNST::GetSize ()
    {
      return super::getPolicy ().size ();
    }

    Ptr<const nnst::Entry>
    NNST::Begin () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    Ptr<nnst::Entry>
    NNST::Begin ()
    {
      super::parent_trie::recursive_iterator item (super::getTrie ());
      super::parent_trie::recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    Ptr<const nnst::Entry>
    NNST::End () const
    {
      return 0;
    }

    Ptr<nnst::Entry>
    NNST::End ()
    {
      return 0;
    }

    Ptr<const nnst::Entry>
    NNST::Next (Ptr<const nnst::Entry> from) const
    {
      if (from == 0) return 0;

      super::parent_trie::const_recursive_iterator item (*StaticCast<const nnst::Entry> (from)->to_iterator ());
      super::parent_trie::const_recursive_iterator end (0);
      for (item++; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    Ptr<nnst::Entry>
    NNST::Next (Ptr<nnst::Entry> from)
    {
      if (from == 0) return 0;

      super::parent_trie::recursive_iterator item (*StaticCast<nnst::Entry> (from)->to_iterator ());
      super::parent_trie::recursive_iterator end (0);
      for (item++; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    Ptr<NNST>
    NNST::GetNNST (Ptr<Object> node)
    {
      return node->GetObject<NNST> ();
    }

    bool
    NNST::FoundName (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      super::iterator item = super::find_exact (prefix);

      if (item == super::end ())
	return false;
      else
	return true;
    }

    std::vector<Address>
    NNST::GetAllPoas (const NNNAddress &prefix)
    {
      NS_LOG_FUNCTION (this << prefix);
      super::iterator item = super::find_exact (prefix);

      if (item == super::end ())
	return std::vector<Address> ();
      else
	return item->payload ()->GetPoAs();
    }

    void
    NNST::NotifyNewAggregate ()
    {
      Object::NotifyNewAggregate ();
    }

    void
    NNST::DoDispose (void)
    {
      clear ();
      Object::DoDispose ();
    }

    Ptr<nnst::Entry>
    NNST::Add (const Ptr<const NNNAddress> &name, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric, char c)
    {
      NS_LOG_FUNCTION ("Full Add" << boost::cref(*name) << boost::cref(*face) << poa << lease_expire << metric);

      // will add entry if doesn't exists, or just return an iterator to the existing entry
      std::pair< super::iterator, bool > result = super::insert (*name, 0);
      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      Ptr<nnst::Entry> newEntry = Create<nnst::Entry> (this, name);

	      newEntry->AddPoA(face, poa, lease_expire, metric);
	      newEntry->SetTrie (result.first);
	      result.first->set_payload (newEntry);
	    }

	  super::modify (result.first,
	                 ll::bind (&nnst::Entry::AddOrUpdateRoutingMetric, ll::_1, face, metric));

	  if (result.second)
	    {
	      // notify forwarding strategy about new NNST entry
	      NS_ASSERT (this->GetObject<ForwardingStrategy> () != 0);
	      this->GetObject<ForwardingStrategy> ()->DidAddNNSTEntry (result.first->payload ());
	    }

	  // If this is a new entry, then the PoA has not been added
	  if (!result.second)
	    {
	      result.first->payload()->AddPoA(face, poa, lease_expire, metric);
	    }

	  return result.first->payload ();
	}
      else
	return 0;
    }

    void
    NNST::RemoveFace (super::parent_trie &item, Ptr<Face> face)
    {
      if (item.payload () == 0) return;
      NS_LOG_FUNCTION (this);

      super::modify (&item,
                     ll::bind (&nnst::Entry::RemoveFace, ll::_1, face));
    }

    void
    NNST::RemovePoA (super::parent_trie &item, Address poa)
    {
      if (item.payload () == 0) return;
      NS_LOG_FUNCTION (this);

      super::modify (&item,
                     ll::bind (&nnst::Entry::RemovePoA, ll::_1, poa));
    }

    void
    NNST::cleanExpired(Ptr<nnst::Entry> item)
    {
      Ptr<const NNNAddress> name = item->GetAddressPtr();
      NS_LOG_FUNCTION (this << boost::cref(*name));

      item->cleanExpired ();

      if (item->isEmpty ())
	Remove (name);
    }

    std::ostream&
    operator<< (std::ostream& os, const NNST &nnst)
    {
      os << "Node " << Names::FindName (nnst.GetObject<Node>()) << std::endl;
      os << std::setw(30) << "3N Address"
	  << std::setw(25) << "PoA Address"
	  << std::setw(5) << " Lease"
	  << std::setw(13) << " Face"
	  << std::setw(4) << " Status"
	  << std::setw(4) << " Cost"
	  << std::setw(5) << " Metric"
	  << std::endl;

      nnst.Print (os);
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */
