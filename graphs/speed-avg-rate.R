#!/usr/bin/Rscript

# speed-avg-rate.R
# Script to create average comparison graphs for Delay and Data rate
#
# Copyright (c) 2015 Waseda University, Sato Laboratory
# Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
#
# speed-avg-rate.R is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# speed-avg-rate.R is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of              
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
# GNU Affero Public License for more details.                                 
#                                                                             
# You should have received a copy of the GNU Affero Public License            
# along with speed-avg-rate.R.  If not, see <http://www.gnu.org/licenses/>.
#

suppressPackageStartupMessages(library (ggplot2))
suppressPackageStartupMessages(library (scales))
suppressPackageStartupMessages(library (optparse))
suppressPackageStartupMessages(library (doBy))
suppressPackageStartupMessages(library (plyr))

types = c("OutInterests", "InData")

rateSummary <- function (file, nodes) {
  
  dt = read.table (file, header=T)
  dt$Node = factor (dt$Node)
  dt$Kilobits <- dt$Kilobytes * 8
  dt$Type = factor (dt$Type)
  dt = subset (dt, Type %in% types)
  dt = subset (dt, Node %in% nodes)
  dt = subset (dt, (Time > 10) & (Time <= (max(dt[1]) -9)))
  
  dt.combined = summaryBy (. ~ Time + Node + Type, data=dt, FUN=sum)
  
  dt.combined = summaryBy (. ~ Node + Type, data=dt.combined, FUN=mean)
  
  return(dt.combined[1,9])
}

connodes = c(0)
prodnodes = c (7)

# Read the 3N Consumer mobility data
# Start with 1.4 m/s data
con3n140 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-1-40-01-001-004.txt", connodes)
# 2.8 m/s data
con3n280 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-2-80-01-001-004.txt", connodes)
# 4.2 m/s data
con3n420 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-4-20-01-001-004.txt", connodes)
# 5.6 m/s data
con3n560 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-5-60-01-001-004.txt", connodes)
# 7 m/s data
con3n700 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-7-00-01-001-004.txt", connodes)
# 8.4 m/s data
con3n840 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-8-40-01-001-004.txt", connodes)
# 9.8 m/s data
con3n980 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-9-80-01-001-004.txt", connodes)
# 11.2 m/s data
con3n1120 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-11-20-01-001-004.txt", connodes)

# Read the NDN Consumer mobility data
# Start with 1.4 m/s data
conNdn140 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-1-40-01-001-004.txt", connodes)
# 2.8 m/s data
conNdn280 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-2-80-01-001-004.txt", connodes)
# 4.2 m/s data
conNdn420 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-4-20-01-001-004.txt", connodes)
# 5.6 m/s data
conNdn560 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-5-60-01-001-004.txt", connodes)
# 7 m/s data
conNdn700 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-7-00-01-001-004.txt", connodes)
# 8.4 m/s data
conNdn840 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-8-40-01-001-004.txt", connodes)
# 9.8 m/s data
conNdn980 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-9-80-01-001-004.txt", connodes)
# 11.2 m/s data
conNdn1120 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-11-20-01-001-004.txt", connodes)

# Read the 3N Producer mobility data
# Start with 1.4 m/s data
prod3n140 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-1-40-01-001-004.txt", prodnodes)
# 2.8 m/s data
prod3n280 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-2-80-01-001-004.txt", prodnodes)
# 4.2 m/s data
prod3n420 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-4-20-01-001-004.txt", prodnodes)
# 5.6 m/s data
prod3n560 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-5-60-01-001-004.txt", prodnodes)
# 7 m/s data
prod3n700 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-7-00-01-001-004.txt", prodnodes)
# 8.4 m/s data
prod3n840 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-8-40-01-001-004.txt", prodnodes)
# 9.8 m/s data
prod3n980 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-9-80-01-001-004.txt", prodnodes)
# 11.2 m/s data
prod3n1120 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-11-20-01-001-004.txt", prodnodes)

# Read the NDN Producer mobility data
# Start with 1.4 m/s data
prodNdn140 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-1-40-01-001-004.txt", prodnodes)
# 2.8 m/s data
prodNdn280 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-2-80-01-001-004.txt", prodnodes)
# 4.2 m/s data
prodNdn420 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-4-20-01-001-004.txt", prodnodes)
# 5.6 m/s data
prodNdn560 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-5-60-01-001-004.txt", prodnodes)
# 7 m/s data
prodNdn700 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-7-00-01-001-004.txt", prodnodes)
# 8.4 m/s data
prodNdn840 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-8-40-01-001-004.txt", prodnodes)
# 9.8 m/s data
prodNdn980 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-9-80-01-001-004.txt", prodnodes)
# 11.2 m/s data
prodNdn1120 = rateSummary("~/git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-11-20-01-001-004.txt", prodnodes)

tbreak <- c(1.4, 2.8, 4.2, 5.6, 7, 8.4, 9.8, 11.2)

speeds <- c( rep (tbreak, 2))
variable <- c (rep ("1", length(tbreak)), rep ("2", length(tbreak)))

conrates <- c(conNdn140, conNdn280, conNdn420, conNdn560, conNdn700, conNdn840, conNdn980, conNdn1120, con3n140, con3n280, con3n420, con3n560, con3n700, con3n840, con3n980, con3n1120)
prodrates <- c(prodNdn140, prodNdn280, prodNdn420, prodNdn560, prodNdn700, prodNdn840, prodNdn980, prodNdn1120, prod3n140, prod3n280, prod3n420, prod3n560, prod3n700, prod3n840, prod3n980, prod3n1120)

avgconrate = data.frame (speeds, variable, conrates)

avgprodrate = data.frame (speeds, variable, prodrates)

theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

grate <- ggplot (avgconrate, aes(colour=variable)) +
  geom_line(aes (x=speeds, y=conrates), size=1) +  
  ggtitle ("Consumer Mobility Avg Data Rate vs Speed") +
  ylab ("Data Rate (Kbits/s)") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (grate)

gprod <- ggplot (avgprodrate, aes(colour=variable)) +
  geom_line(aes (x=speeds, y=prodrates), size=1) +  
  ggtitle ("Consumer with Producer Mobility Avg Data Rate vs Speed") +
  ylab ("Data Rate (Kbits/s)") +
  xlab ("Speed (m/s)") +
  scale_colour_discrete(name = "Strategies", labels = c("NDN Smart Flooding", "3N + Smart Flooding")) +
  scale_x_continuous (breaks=tbreak)

print (gprod)