#!/usr/bin/Rscript

# speed-avg-delay.R
# Script to create average comparison graphs for Delay and Data rate
#
# Copyright (c) 2015 Waseda University, Sato Laboratory
# Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
#
# speed-avg-delay.R is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# speed-avg-delay.R is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of              
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
# GNU Affero Public License for more details.                                 
#                                                                             
# You should have received a copy of the GNU Affero Public License            
# along with speed-avg-delay.R.  If not, see <http://www.gnu.org/licenses/>.
#

suppressPackageStartupMessages(library (ggplot2))
suppressPackageStartupMessages(library (scales))
suppressPackageStartupMessages(library (optparse))
suppressPackageStartupMessages(library (doBy))
suppressPackageStartupMessages(library (plyr))

allFilter <- function (file, nodes=c())
{
  dt = read.table (file, header=T)
  dt$Node = factor (dt$Node)
  dt$Type = factor (dt$Type)
  
  # Filter for a particular node
  if (length(nodes) > 0) {
    dt = subset (dt, Node %in% nodes)
  }
  
  dt = subset (dt, Type %in% c("FullDelay"))
  
  return (dt)
}

speedSummary <- function (file, nodes=c())
{
  dt = allFilter (file, nodes)
  
  dt.comb = summaryBy (. ~ Type, data=dt, FUN=mean)
  
  return (dt.comb[1,5])
}

seqSummary <- function (file, nodes=c())
{
  dt = allFilter (file, nodes)
  
  return (max (dt$SeqNo))
}

# Read the 3N Consumer mobility data
# Start with 1.4 m/s data
con3n140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-1-40-01-001-004.txt")
# 2.8 m/s data
con3n280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-2-80-01-001-004.txt")
# 4.2 m/s data
con3n420 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-4-20-01-001-004.txt")
# 5.6 m/s data
con3n560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-5-60-01-001-004.txt")
# 7 m/s data
con3n700 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-7-00-01-001-004.txt")
# 8.4 m/s data
con3n840 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-8-40-01-001-004.txt")
# 9.8 m/s data
con3n980 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-9-80-01-001-004.txt")
# 11.2 m/s data
con3n1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-11-20-01-001-004.txt")

# Read the NDN Consumer mobility data
# Start with 1.4 m/s data
conNdn140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-1-40-01-001-004.txt")
# 2.8 m/s data
conNdn280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-2-80-01-001-004.txt")
# 4.2 m/s data
conNdn420 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-4-20-01-001-004.txt")
# 5.6 m/s data
conNdn560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-5-60-01-001-004.txt")
# 7 m/s data
conNdn700 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-7-00-01-001-004.txt")
# 8.4 m/s data
conNdn840 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-8-40-01-001-004.txt")
# 9.8 m/s data
conNdn980 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-9-80-01-001-004.txt")
# 11.2 m/s data
conNdn1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-11-20-01-001-004.txt")

# Read the 3N Producer mobility data
# Start with 1.4 m/s data
prod3n140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prod3n280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-2-80-01-001-004.txt")
# 4.2 m/s data
prod3n420 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-4-20-01-001-004.txt")
# 5.6 m/s data
prod3n560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-5-60-01-001-004.txt")
# 7 m/s data
prod3n700 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-7-00-01-001-004.txt")
# 8.4 m/s data
prod3n840 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-8-40-01-001-004.txt")
# 9.8 m/s data
prod3n980 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-9-80-01-001-004.txt")
# 11.2 m/s data
prod3n1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-11-20-01-001-004.txt")

# Read the NDN Producer mobility data
# Start with 1.4 m/s data
prodNdn140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prodNdn280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-2-80-01-001-004.txt")
# 4.2 m/s data
prodNdn420 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-4-20-01-001-004.txt")
# 5.6 m/s data
prodNdn560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-5-60-01-001-004.txt")
# 7 m/s data
prodNdn700 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-7-00-01-001-004.txt")
# 8.4 m/s data
prodNdn840 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-8-40-01-001-004.txt")
# 9.8 m/s data
prodNdn980 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-9-80-01-001-004.txt")
# 11.2 m/s data
prodNdn1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-11-20-01-001-004.txt")

theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

tbreak <- c(1.4, 2.8, 4.2, 5.6, 7, 8.4, 9.8, 11.2)

speeds <- c( rep (tbreak, 2))
variable <- c (rep ("1", length(tbreak)), rep ("2", length(tbreak)))

condelays <- c(conNdn140, conNdn280, conNdn420, conNdn560, conNdn700, conNdn840, conNdn980, conNdn1120, con3n140, con3n280, con3n420, con3n560, con3n700, con3n840, con3n980, con3n1120)
proddelays <- c(prodNdn140, prodNdn280, prodNdn420, prodNdn560, prodNdn700, prodNdn840, prodNdn980, prodNdn1120, prod3n140, prod3n280, prod3n420, prod3n560, prod3n700, prod3n840, prod3n980, prod3n1120)

avgcondelay = data.frame (speeds, variable, condelays)
avgproddelay = data.frame (speeds, variable, proddelays)

grate <- ggplot (avgcondelay, aes(colour=variable)) +
  geom_line(aes (x=speeds, y=condelays), size=1) +  
  ggtitle ("Consumer Mobility Avg Network Delay vs Speed") +
  ylab ("Delay [Seconds]") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (grate)

grate <- ggplot (avgproddelay, aes(colour=variable)) +
  geom_line(aes (x=speeds, y=proddelays), size=1) +  
  ggtitle ("Consumer with Producer Mobility Avg Network Delay vs Speed") +
  ylab ("Delay [Seconds]") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (grate)

avg3nproddelay = subset (avgproddelay, variable %in% c("2"))

grate <- ggplot (avg3nproddelay) +
  geom_line(aes (x=speeds, y=proddelays), size=1) +  
  ggtitle ("Consumer with Producer Mobility Avg Network Delay vs Speed") +
  ylab ("Delay [Seconds]") +
  xlab ("Speed (m/s)") +
  scale_x_continuous (breaks=tbreak)

print (grate)

# Read the 3N Consumer mobility data
# Start with 1.4 m/s data
conSeq3n140 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-1-40-01-001-004.txt")
# 2.8 m/s data
conSeq3n280 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-2-80-01-001-004.txt")
# 4.2 m/s data
conSeq3n420 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-4-20-01-001-004.txt")
# 5.6 m/s data
conSeq3n560 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-5-60-01-001-004.txt")
# 7 m/s data
conSeq3n700 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-7-00-01-001-004.txt")
# 8.4 m/s data
conSeq3n840 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-8-40-01-001-004.txt")
# 9.8 m/s data
conSeq3n980 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-9-80-01-001-004.txt")
# 11.2 m/s data
conSeq3n1120 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-11-20-01-001-004.txt")

# Read the NDN Consumer mobility data
# Start with 1.4 m/s data
conSeqNdn140 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-1-40-01-001-004.txt")
# 2.8 m/s data
conSeqNdn280 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-2-80-01-001-004.txt")
# 4.2 m/s data
conSeqNdn420 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-4-20-01-001-004.txt")
# 5.6 m/s data
conSeqNdn560 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-5-60-01-001-004.txt")
# 7 m/s data
conSeqNdn700 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-7-00-01-001-004.txt")
# 8.4 m/s data
conSeqNdn840 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-8-40-01-001-004.txt")
# 9.8 m/s data
conSeqNdn980 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-9-80-01-001-004.txt")
# 11.2 m/s data
conSeqNdn1120 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-11-20-01-001-004.txt")

# Read the 3N Producer mobility data
# Start with 1.4 m/s data
prodSeq3n140 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prodSeq3n280 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-2-80-01-001-004.txt")
# 4.2 m/s data
prodSeq3n420 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-4-20-01-001-004.txt")
# 5.6 m/s data
prodSeq3n560 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-5-60-01-001-004.txt")
# 7 m/s data
prodSeq3n700 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-7-00-01-001-004.txt")
# 8.4 m/s data
prodSeq3n840 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-8-40-01-001-004.txt")
# 9.8 m/s data
prodSeq3n980 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-9-80-01-001-004.txt")
# 11.2 m/s data
prodSeq3n1120 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-11-20-01-001-004.txt")

# Read the NDN Producer mobility data
# Start with 1.4 m/s data
prodSeqNdn140 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prodSeqNdn280 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-2-80-01-001-004.txt")
# 4.2 m/s data
prodSeqNdn420 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-4-20-01-001-004.txt")
# 5.6 m/s data
prodSeqNdn560 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-5-60-01-001-004.txt")
# 7 m/s data
prodSeqNdn700 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-7-00-01-001-004.txt")
# 8.4 m/s data
prodSeqNdn840 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-8-40-01-001-004.txt")
# 9.8 m/s data
prodSeqNdn980 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-9-80-01-001-004.txt")
# 11.2 m/s data
prodSeqNdn1120 = seqSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-11-20-01-001-004.txt")

conSeq <- c(conSeqNdn140, conSeqNdn280, conSeqNdn420, conSeqNdn560, conSeqNdn700, conSeqNdn840, conSeqNdn980, conSeqNdn1120, conSeq3n140, conSeq3n280, conSeq3n420, conSeq3n560, conSeq3n700, conSeq3n840, conSeq3n980, conSeq3n1120)
prodSeq <- c(prodSeqNdn140, prodSeqNdn280, prodSeqNdn420, prodSeqNdn560, prodSeqNdn700, prodSeqNdn840, prodSeqNdn980, prodSeqNdn1120, prodSeq3n140, prodSeq3n280, prodSeq3n420, prodSeq3n560, prodSeq3n700, prodSeq3n840, prodSeq3n980, prodSeq3n1120)

avgconSeq = data.frame (speeds, variable, conSeq)
avgprodSeq = data.frame (speeds, variable, prodSeq)

grate <- ggplot (avgconSeq, aes(x=speeds, y=conSeq)) +
  geom_line(aes (colour=variable), size=1) +
  ggtitle ("Consumer Mobility Received Sequence vs Speed") +
  ylab ("Sequence Number Received") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (grate)

grate <- ggplot (avgprodSeq, aes(x=speeds, y=prodSeq)) +
  geom_line(aes (colour=variable), size=1) +  
  ggtitle ("Consumer with Producer Mobility Received Sequence vs Speed") +
  ylab ("Sequence Number Received") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (grate)
