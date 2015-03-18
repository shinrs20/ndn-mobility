/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Takahiro Miyamoto <mt3.mos@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnpt.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnpt.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnpt.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-nnpt.h"

NS_LOG_COMPONENT_DEFINE ("nnn.NNPT");

namespace ns3
{
  namespace nnn
  {
    TypeId
    NNPT::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::NNPT") // cheating ns3 object system
    		    .SetParent<Object> ()
		    .SetGroupName ("Nnn")
		    ;
      return tid;
    }

    NNPT::NNPT() {
    }

    NNPT::~NNPT() {
    }

    void
    NNPT::addEntry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire)
    {
      NS_LOG_FUNCTION (this);

      if (!foundOldName(oldName) && !foundOldName(newName))
        {
          container.insert(NNPTEntry(oldName, newName, lease_expire));

          Simulator::Schedule(lease_expire, &NNPT::cleanExpired, this);
        }
      else
        {
          NS_LOG_INFO("Found either " << *oldName << " or " << *newName << " already in NNPT");
        }
    }

    void
    NNPT::addEntry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire, Time renew)
    {
      NS_LOG_FUNCTION (this);

      if (!foundOldName(oldName) && !foundOldName(newName))
        {
          container.insert(NNPTEntry(oldName, newName, lease_expire, renew));

          Simulator::Schedule(lease_expire, &NNPT::cleanExpired, this);
        }
      else
        {
          NS_LOG_INFO("Found either " << *oldName << " or " << *newName << " already in NNPT");
        }

    }

    void
    NNPT::addEntry (NNPTEntry nnptEntry)
    {
      NS_LOG_FUNCTION (this);

      if (!foundOldName(nnptEntry.m_oldName) && !foundOldName(nnptEntry.m_newName))
        {
          container.insert(nnptEntry);

          Simulator::Schedule(nnptEntry.m_lease_expire, &NNPT::cleanExpired, this);
        }
      else
        {
          NS_LOG_INFO("Found either " << *nnptEntry.m_oldName << " or " << *nnptEntry.m_newName << " already in NNPT");
        }
    }

    void
    NNPT::deleteEntry (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this);
      NNPTEntry tmp = findEntry (oldName);
      container.erase(tmp);
    }

    void
    NNPT::deleteEntry (NNPTEntry nnptEntry)
    {
      NS_LOG_FUNCTION (this);
      container.erase(nnptEntry);
    }

    bool
    NNPT::foundOldName (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& names_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = names_index.find(name);

      if (it == names_index.end())
	return false;
      else
	return true;
    }

    bool
    NNPT::foundNewName (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_newname& names_index = container.get<newname> ();
      pair_set_by_newname::iterator it = names_index.find(name);

      if (it == names_index.end())
	return false;
      else
	return true;
    }

    const NNNAddress&
    NNPT::findPairedName (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this);

      return *findPairedNamePtr(oldName);
    }

    const NNNAddress&
    NNPT::findPairedOldName (Ptr<const NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this);

      return *findPairedOldNamePtr(newName);
    }

    Ptr<const NNNAddress>
    NNPT::findPairedNamePtr (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(oldName);

      if (it != pair_index.end())
	{
	  NNPTEntry tmp;
	  // Check if there is a newer entry
	  while (true)
	    {
	      tmp = *it;
	      it = pair_index.find(tmp.m_newName);
	      if (it == pair_index.end())
		break;
	    }
	  return tmp.m_newName;
	}
      else
	{
	  return oldName;
	}
    }

    Ptr<const NNNAddress>
    NNPT::findPairedOldNamePtr (Ptr<const NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_newname& pair_index = container.get<newname> ();
      pair_set_by_newname::iterator it = pair_index.find(newName);

      if (it != pair_index.end ())
	{
	  NNPTEntry tmp;
	  // Check if there is a newer entry
	  while (true)
	    {
	      tmp = *it;
	      it = pair_index.find (tmp.m_oldName);
	      if (it == pair_index.end ())
		break;
	    }
	  return tmp.m_oldName;
	}
      else
	{
	  return newName;
	}
    }

    NNPTEntry
    NNPT::findEntry (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(name);

      if (it != pair_index.end())
	{
	  return *it;
	}
      else
	{
	  return NNPTEntry ();
	}
    }

    void
    NNPT::updateLeaseTime (Ptr<const NNNAddress> oldName, Time lease_expire)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(oldName);

      if (it != pair_index.end())
	{
	  NNPTEntry tmp = *it;

	  tmp.m_lease_expire = lease_expire;
	  tmp.m_renew = lease_expire - Seconds (1);

	  if (pair_index.replace(it, tmp))
	    {
	      Simulator::Schedule(lease_expire, &NNPT::cleanExpired, this);
	    }
	}
    }

    void
    NNPT::updateLeaseTime (Ptr<const NNNAddress> oldName, Time lease_expire, Time renew)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(oldName);

      if (it != pair_index.end())
	{
	  NNPTEntry tmp = *it;

	  tmp.m_lease_expire = lease_expire;
	  tmp.m_renew = renew;

	  if (pair_index.replace(it, tmp))
	    {
	      Simulator::Schedule(lease_expire, &NNPT::cleanExpired, this);
	    }
	}
    }

    uint32_t
    NNPT::size ()
    {
      NS_LOG_FUNCTION (this);
      return container.size();
    }

    bool
    NNPT::isEmpty ()
    {
      NS_LOG_FUNCTION (this);
      return (container.size() == 0);
    }

    Time
    NNPT::findNameExpireTime (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      NNPTEntry tmp = findEntry(name);

      return tmp.m_lease_expire;
    }

    Time
    NNPT::findNameExpireTime (NNPTEntry nnptEntry)
    {
      NS_LOG_FUNCTION (this);
      return nnptEntry.m_lease_expire;
    }

    void
    NNPT::cleanExpired ()
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_lease& lease_index = container.get<st_lease> ();
      Time now = Simulator::Now();

      //std::cout << "Deleting expired entries at " << now << std::endl;

      pair_set_by_lease::iterator it = lease_index.begin();

      while (! isEmpty())
	{
	  if (it->m_lease_expire <= now)
	    {
	      deleteEntry(*it);
	      break;
	    }
	  ++it;
	}
    }

    void
    NNPT::printByAddress ()
    {
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.begin();

      std::cout << "Old Address\t| New Address\t| Lease Expire\t| Renew" << std::endl;
      std::cout << "--------------------------------------------------------" << std::endl;

      while (it != pair_index.end())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    void
    NNPT::printByLease ()
    {
      pair_set_by_lease& lease_index = container.get<st_lease> ();
      pair_set_by_lease::iterator it = lease_index.begin();

      std::cout << "NNN Address\t| New Address\t| Lease Expire\t| Renew" << std::endl;
      std::cout << "--------------------------------------------------------" << std::endl;

      while (it != lease_index.end ())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    std::ostream&
    operator<< (std::ostream& os, const NNPT &nnpt)
    {
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */
