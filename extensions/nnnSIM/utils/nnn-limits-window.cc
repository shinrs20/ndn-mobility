/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-limits-window.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-limits-window.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-limits-window.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-limits-window.h"

#include <ns3-dev/ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("nnn.Limits.Window");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (LimitsWindow);

    TypeId
    LimitsWindow::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::nnn::Limits::Window")
	.SetGroupName ("Nnn")
	.SetParent <Limits> ()
	.AddConstructor <LimitsWindow> ()

	.AddTraceSource ("CurMaxLimit",
	                 "Current maximum limit",
	                 MakeTraceSourceAccessor (&LimitsWindow::m_curMaxLimit))

	                 .AddTraceSource ("Outstanding",
	                                  "Number of outstanding interests",
	                                  MakeTraceSourceAccessor (&LimitsWindow::m_outstanding))
	                                  ;
      return tid;
    }

    void
    LimitsWindow::UpdateCurrentLimit (double limit)
    {
      NS_ASSERT_MSG (limit >= 0.0, "Limit should be greater or equal to zero");

      m_curMaxLimit = std::min (limit, GetMaxRate () * GetMaxDelay ());
    }

    bool
    LimitsWindow::IsBelowLimit ()
    {
      if (!IsEnabled ()) return true;

      return (m_curMaxLimit - m_outstanding >= 1.0);
    }

    void
    LimitsWindow::BorrowLimit ()
    {
      if (!IsEnabled ()) return;

      NS_ASSERT_MSG (m_curMaxLimit - m_outstanding >= 1.0, "Should not be possible, unless we IsBelowLimit was not checked correctly");
      m_outstanding += 1;
    }

    void
    LimitsWindow::ReturnLimit ()
    {
      if (!IsEnabled ()) return;

      NS_ASSERT_MSG (m_outstanding >= (uint32_t)1, "Should not be possible, unless we decreasing this number twice somewhere");
      m_outstanding -= 1;

      FireAvailableSlotCallback ();
    }

  } // namespace nnn
} // namespace ns3
