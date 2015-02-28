/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-address.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-address.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-address.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <ctype.h>

#include "nnn-address.h"
#include "error.h"

#include "../wire/wire-nnnsim.h"

using namespace std;

NNN_NAMESPACE_BEGIN

ATTRIBUTE_HELPER_CPP (NNNAddress);

///////////////////////////////////////////////////////////////////////////////
//                              CONSTRUCTORS                                 //
///////////////////////////////////////////////////////////////////////////////

NNNAddress::NNNAddress ()
{
}

NNNAddress::NNNAddress (const NNNAddress &other)
{
  m_address_comp = other.m_address_comp;
}

// Create a valid NNN address
// No more than 16 hexadecimal characters with a maximum of 15 "."
NNNAddress::NNNAddress (const string &name)
{
  string::const_iterator i = name.begin ();
  string::const_iterator end = name.end ();

  // Check that we have only hexadecimal characters and dots
  boost::regex e("[^.0-9a-fA-F]");
  boost::match_results<std::string::const_iterator> what;
  boost::match_flag_type flags = boost::match_default;

  if (boost::regex_search(i, end, what, e, flags))
    {
      BOOST_THROW_EXCEPTION(error::NNNAddress () << error::msg("NNN address should be composed of only hexadecimal characters and dots!"));
    }

  // Check that string has less than 15 dots.
  int dotcount = count(i, end, SEP);

  if (dotcount > 15)
    {
      BOOST_THROW_EXCEPTION(error::NNNAddress () << error::msg("NNN address should not have more than 15 '.'"));
    }

  // With all the basic checks done, now attempt to get the components in order
  while (i != end)
    {
      int consecutivedot = 0;
      // If the start is with one or more SEP then move forward
      while (i != end && *i == SEP)
	{
	  consecutivedot++;
	  i++;
	}

      if (consecutivedot > 1)
	BOOST_THROW_EXCEPTION(error::NNNAddress () << error::msg("NNN address dot must be followed by a hexadecimal number!"));

      if (consecutivedot != 0 && i == end)
	BOOST_THROW_EXCEPTION(error::NNNAddress () << error::msg("NNN address dot must be followed by a hexadecimal number!"));

      if (i == end)
	break;

      // Read until the next separator
      string::const_iterator nextDot = std::find (i, end, SEP);

      // Create a new name component until the spot found
      name::Component comp;

      appendBySwap(comp.fromDotHexStr(i, nextDot));

      // Update the location and continue
      i = nextDot;
    }
}

NNNAddress::NNNAddress (const std::vector<name::Component> name)
{
  m_address_comp = name;
}

const name::Component &
NNNAddress::get (int index) const
{
  if (index < 0)
    {
      index = size () - (-index);
    }

  if (static_cast<unsigned int> (index) >= size ())
    {
      BOOST_THROW_EXCEPTION (error::NNNAddress ()
      << error::msg ("Index out of range")
      << error::pos (index));
    }
  return m_address_comp [index];
}

name::Component &
NNNAddress::get (int index)
{
  if (index < 0)
    {
      index = size () - (-index);
    }

  if (static_cast<unsigned int> (index) >= size())
    {
      BOOST_THROW_EXCEPTION (error::NNNAddress ()
      << error::msg ("Index out of range")
      << error::pos (index));
    }
  return m_address_comp [index];
}

NNNAddress &
NNNAddress::operator= (const NNNAddress &other)
{
  m_address_comp = other.m_address_comp;
  return *this;
}

NNNAddress
NNNAddress::getName () const
{
  if (isEmpty()) {
      return NNNAddress ();
  } else
    {
      // Copy the old name
      std::vector<name::Component> sectorName (m_address_comp);

      return NNNAddress (sectorName);
    }
}

NNNAddress
NNNAddress::getSectorName () const
{
  if (isEmpty()) {
      return NNNAddress ();
  } else
    {
      // Copy the old name
      std::vector<name::Component> sectorName (m_address_comp);

      // Eliminate the last position
      sectorName.pop_back();

      return NNNAddress (sectorName);
    }
}

std::string
NNNAddress::toDotHex () const
{
  ostringstream os;
  toDotHex (os);
  return os.str ();
}

void
NNNAddress::toDotHex (std::ostream &os) const
{
  for (NNNAddress::const_iterator comp = begin (); comp != end (); comp++)
    {
      comp->toHex (os);
      // Do not write SEP at the final round
      if (comp+1 == end ())
	break;
      else
	os << SEP;
    }
}

int
NNNAddress::compare (const NNNAddress &name) const
{
  NNNAddress::const_iterator i = this->begin ();
  NNNAddress::const_iterator j = name.begin ();

  for (; i != this->end () && j != name.end (); i++, j++)
    {
      int res = i->compare (*j);
      if (res == 0)
	continue;
      else
	return res;
    }

  // If prefixes are equal
  if (i == this->end () && j == name.end ())
    return 0;

  return (i == this->end ()) ? -1 : +1;
}

int
NNNAddress::compareLabels(const NNNAddress & name) const
{
  NNNAddress::const_iterator i = this->begin ();
  NNNAddress::const_iterator j = name.begin ();

  for (; i != this->end () && j != name.end (); i++, j++)
    {
      if (i->toNumber () == j->toNumber ())
	continue;
      else
	return (i->toNumber() > j->toNumber ()) ? +1 : -1;
    }

  // The prefixes are the same, test the final label
  if (i == this->end () && j == name.end ())
    {
      i--;
      j--;
      if (i->toNumber() == j->toNumber())
	{
	  return 0;
	}
      else
	return (i->toNumber() > j->toNumber ()) ? +1 : -1;
    }

  return (i == this->end ()) ? +1 : -1;
}

bool
NNNAddress::isSameSector (const NNNAddress &name) const
{
  NNNAddress currSec = getSectorName();
  NNNAddress nameSec = name.getSectorName();

  int res = currSec.compare(nameSec);

  return (res == 0);
}
bool
NNNAddress::isSubSector (const NNNAddress &name) const
{

  if (name.size () <= this->size ())
    {
      NNNAddress::const_iterator i = this->begin ();
      NNNAddress::const_iterator j = name.begin ();

      bool ret = true;

      for (; i != this->end () && j != name.end (); i++, j++)
	{
	  if (j->toNumber () != i->toNumber ())
	    {
	      ret = false;
	      break;
	    }
	}

      return ret;
    }
  else
    return false;
}

bool
NNNAddress::isParentSector (const NNNAddress &name) const
{
  if (name.size () > this->size ())
    {
      NNNAddress::const_iterator i = this->begin ();
      NNNAddress::const_iterator j = name.begin ();

      bool ret = true;

      for (; i != this->end () && j != name.end (); i++, j++)
	{
	  if (j->toNumber () != i->toNumber ())
	    {
	      ret = false;
	      break;
	    }
	}

      return ret;
    }
  else
    return false;
}

bool
NNNAddress::isToplvlSector () const
{
  return (size () == 1);
}

bool
NNNAddress::isOneLabel () const
{
  return isToplvlSector ();
}

NNNAddress
NNNAddress::getLastLabel () const
{
  if (isEmpty()) {
      return NNNAddress ();
  } else
    {
      return NNNAddress ().append(m_address_comp.at(size() -1));
    }
}

bool
NNNAddress::isEmpty () const
{
  return (size () == 0);
}

NNNAddress
NNNAddress::getClosestSector (const NNNAddress &name) const
{
  // Name is a usually a destination, thus in the worse of cases, the
  // top level to get to the name is top level of name.
  if (isToplvlSector () || name.isToplvlSector ())
    return NNNAddress ().append (name[0]);

  // Compare
  int res = compare (name);

  // If the same, then the top level is the closest sector
  if (res == 0)
    {
      return NNNAddress (m_address_comp);
      // The address given is smaller than the one we have
    }
  else if ( res == 1)
    {
      return getSectorName ().getClosestSector(name);
      // The address given is bigger than the one we have
    }
  else
    {
      return getClosestSector (name.getSectorName ());
    }
}

NNNAddress
NNNAddress::operator+ (const NNNAddress &name) const
{
  NNNAddress newName;
  newName.append(*this).append(name);
  return newName;
}

uint8_t
NNNAddress::GetType (void)
{
  static uint8_t type = Address::Register () ;
  return type;
}

Address
NNNAddress::ConvertTo (void) const
{
  // We use the functions existing in Wire to Serialize and pass to Address
  Buffer buf;
  size_t nameBytes = wire::NnnSim::SerializedSizeName(*this);
  buf.AddAtStart(nameBytes);
  // Create a uint8_t array, required by Address
  uint8_t namebuf[nameBytes];
  // Begin the buffer iterator
  Buffer::Iterator i = buf.Begin ();

  // Serialize it to our buffer
  wire::NnnSim::SerializeName (i, *this);

  i.Write (namebuf, nameBytes);

  return Address(GetType (), namebuf, nameBytes);
}

NNNAddress
NNNAddress::ConvertFrom (const Address &address)
{

  // Get the length of the address
  uint8_t len = address.GetLength ();

  // Create a 8 bit array of the length obtained
  uint8_t namebuf[len];

  // Copy the address information to the array
  address.CopyTo(namebuf);


  // Create a Buffer class
  Buffer buf;
  buf.AddAtStart(len);

  Buffer::Iterator i = buf.Begin ();
  // Read the array into the Buffer class
  i.Read(namebuf, len);

  // Deserialize the information
  Ptr<NNNAddress> tmp = wire::NnnSim::DeserializeName(i);

  return NNNAddress(tmp->toDotHex());
}

int
NNNAddress::distance (const NNNAddress &name) const
{
  //std::cout << "Comparing " << *this << " with " << name << std::endl;
  int res = compare(name);

  if (res == 0)
    return 0;
  else
    {
      NNNAddress::const_iterator i = this->begin ();
      NNNAddress::const_iterator j = name.begin ();
      if (isToplvlSector())
	{
	  int fin = name.size ();
	  if (i->compare(*j) == 0)
	    return fin - 1;
	  else
	    return fin;
	}

      if (name.isToplvlSector())
	{
	  int fin = size ();
	  if (i->compare(*j) == 0)
	    return fin - 1;
	  else
	    return fin;
	}

      int s1 = size ();
      int s2 = name.size ();
      if (s1 == s2)
	{
	  if (isSameSector(name))
	    {
	      return 2;
	    }

	  if (res == 1)
	    {
	      return getSectorName ().distance(name) + 1;
	    }
	  else
	    {
	      return distance(name.getSectorName ()) + 1;
	    }
	}
      else if (s1 > s2)
	{
	  return getSectorName ().distance(name) + 1;
	}
      else
	{
	  return distance(name.getSectorName ()) + 1;
	}
    }
}

bool
NNNAddress::canAppendComponent()
{
  return (size() < MAXCOMP);
}

NNN_NAMESPACE_END
