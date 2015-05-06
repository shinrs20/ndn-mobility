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

speedSummary <- function (file, nodes=c())
{
  dt = read.table (file, header=T)
  dt$Node = factor (dt$Node)
  dt$Type = factor (dt$Type)
  
  # Filter for a particular node
  if (length(nodes) > 0) {
    dt = subset (dt, Node %in% nodes)
  }
  
  dt = subset (dt, Type %in% c("FullDelay"))
  
  dt.comb = summaryBy (. ~ Type, data=dt, FUN=mean)
  
  return (dt.comb[1,5])
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
