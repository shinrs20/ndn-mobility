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
data3n140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-1-40-01-001-004.txt")
# 2.8 m/s data
data3n280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-2-80-01-001-004.txt")
# 5.6 m/s data
data3n560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-5-60-01-001-004.txt")
# 11.2 m/s data
data3n1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-con-11-20-01-001-004.txt")

# Read the NDN Consumer mobility data
# Start with 1.4 m/s data
datandn140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-1-40-01-001-004.txt")
# 2.8 m/s data
datandn280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-2-80-01-001-004.txt")
# 5.6 m/s data
datandn560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-5-60-01-001-004.txt")
# 11.2 m/s data
datandn1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-con-11-20-01-001-004.txt")

# Read the 3N Producer mobility data
# Start with 1.4 m/s data
prod3n140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prod3n280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-2-80-01-001-004.txt")
# 5.6 m/s data
prod3n560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-5-60-01-001-004.txt")
# 11.2 m/s data
prod3n1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-3n-prod-11-20-01-001-004.txt")

# Read the NDN Producer mobility data
# Start with 1.4 m/s data
prodndn140 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-1-40-01-001-004.txt")
# 2.8 m/s data
prodndn280 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-2-80-01-001-004.txt")
# 5.6 m/s data
prodndn560 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-5-60-01-001-004.txt")
# 11.2 m/s data
prodndn1120 = speedSummary("~/git/ndn-mobility/results/MobilityICC-app-delays-smart-prod-11-20-01-001-004.txt")

theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

tbreak <- c(1.4, 2.8, 5.6, 11.2)

speeds <- c( rep (tbreak, 2))
variable <- c (rep ("1", 4), rep ("2", 4))

condelays <- c(datandn140, datandn280, datandn560, datandn1120, data3n140, data3n280, data3n560, data3n1120)
proddelays <- c(prodndn140, prodndn280, prodndn560, prodndn1120, prod3n140, prod3n280, prod3n560, prod3n1120)

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
