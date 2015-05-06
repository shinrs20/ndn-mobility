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
data3n140 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-1-40-01-001-004.txt", connodes)
# 2.8 m/s data
data3n280 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-2-80-01-001-004.txt", connodes)
# 5.6 m/s data
data3n560 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-5-60-01-001-004.txt", connodes)
# 11.2 m/s data
data3n1120 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-con-11-20-01-001-004.txt", connodes)

# Read the NDN Consumer mobility data
# Start with 1.4 m/s data
datandn140 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-1-40-01-001-004.txt", connodes)
# 2.8 m/s data
datandn280 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-2-80-01-001-004.txt", connodes)
# 5.6 m/s data
datandn560 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-5-60-01-001-004.txt", connodes)
# 11.2 m/s data
datandn1120 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-con-11-20-01-001-004.txt", connodes)

# Read the 3N Producer mobility data
# Start with 1.4 m/s data
prod3n140 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-1-40-01-001-004.txt", prodnodes)
# 2.8 m/s data
prod3n280 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-2-80-01-001-004.txt", prodnodes)
# 5.6 m/s data
prod3n560 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-5-60-01-001-004.txt", prodnodes)
# 11.2 m/s data
prod3n1120 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-3n-prod-11-20-01-001-004.txt", prodnodes)

# Read the NDN Producer mobility data
# Start with 1.4 m/s data
prodndn140 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-1-40-01-001-004.txt", prodnodes)
# 2.8 m/s data
prodndn280 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-2-80-01-001-004.txt", prodnodes)
# 5.6 m/s data
prodndn560 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-5-60-01-001-004.txt", prodnodes)
# 11.2 m/s data
prodndn1120 = rateSummary("git/ndn-mobility/results/MobilityICC-rate-trace-smart-prod-11-20-01-001-004.txt", prodnodes)

tbreak <- c(1.4, 2.8, 5.6, 11.2)

speeds <- c( rep (tbreak, 2))
variable <- c (rep ("1", 4), rep ("2", 4))

conrates <- c(datandn140, datandn280, datandn560, datandn1120, data3n140, data3n280, data3n560, data3n1120)
prodrates <- c(prodndn140, prodndn280, prodndn560, prodndn1120, prod3n140, prod3n280, prod3n560, prod3n1120)

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