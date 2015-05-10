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

# set some reasonable defaults for the options that are needed
option_list <- list (
  make_option(c("-d", "--directory"), type="character", default="~/git/ndn-mobility/results",
              help="Directory which holds the raw data.\n\t\t[Default \"%default\"]"),
  make_option(c("-o", "--output"), type="character", default=".",
              help="Output directory for graphs.\n\t\t[Default \"%default\"]"),
  make_option(c("--e1"), type="character", default="0",
              help="Consumer node data to graph. Can be a comma separated list.\n\t\tDefault graphs data for all nodes."),
  make_option(c("--e2"), type="character", default="7",
              help="Producer node data to graph. Can be a comma separated list.\n\t\tDefault graphs data for all nodes."),
  make_option(c("-m", "--speeds"), type="character", default="1.4,2.8,4.2,5.6,7,8.4,9.8,11.2",
              help="Mobile speeds to compare.\n\t\t[Default \"%default\"]"),
  make_option(c("-t", "--title"), type="character", default="Avg Data Rate vs Speed",
              help="Title for the graph"),
  make_option(c("--str1"), type="character", default="NDN Smart Flooding",
              help="Legend title for -f file data\n\t\t[Default \"%default\"]"),
  make_option(c("--str2"), type="character", default="3N + Smart Flooding",
              help="Legend title for -c file data\n\t\t[Default \"%default\"]"),
  make_option(c("-c", "--consumer"), action="store_true", default=FALSE,
              help="Tell the script to graph Consumer mobility data"),
  make_option(c("-p", "--producer"), action="store_true", default=FALSE,
              help="Tell the script to graph Producer mobility data"),
  make_option(c("-s", "--scenario"), type="character", default="MobilityICC",
              help="Name of the scenario to produce graphs for.\n\t\t[Default \"%default\"]"),
  make_option(c("-x", "--extension"), type="character", default="01-001-004.txt",
              help="The subfix of the extension files to use.\n\t\t[Default \"%default\"]")
)

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

# Load the parser
opt = parse_args(OptionParser(option_list=option_list, description="Creates summary Rate graphs for 3N vs NDN"))

cat (sprintf ("Creating comparison graphs for speeds: %s\n", opt$speeds))

# Read the speeds
filspeeds = unlist(strsplit(opt$speeds, ","))

# Read the speeds as doubles
tbreak = as.double (filspeeds)

strSpeeds = character (length(tbreak))

for (i in 1:length(tbreak))
{
  strSpeeds[i] = gsub ("\\.", "-", sprintf("%.2f", tbreak[i]))
}

connodes = unlist(strsplit(opt$e1, ","))
prodnodes = unlist(strsplit(opt$e2, ","))

# Create the resulting speed vectors
speeds <- c( rep (tbreak, 2))

variable <- c (rep ("1", length(tbreak)), rep ("2", length(tbreak)))

# Create the complete vector for rates
conRateSpeeds = double (length(speeds))
prodRateSpeeds = double(length(speeds))

theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

if (opt$consumer)
{
  cat(sprintf ("Creating Average Data rate vs Speed for %s Consumer %s mobility graph\n", opt$scenario, opt$e1))
  for (i in 1:length(strSpeeds))
  {
    file3n = sprintf("%s/%s-rate-trace-3n-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
    fileNdn = sprintf("%s/%s-rate-trace-smart-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
    
    conRateSpeeds[i] = rateSummary(fileNdn, connodes)
    conRateSpeeds[i+length(strSpeeds)] = rateSummary (file3n, connodes)
  }
  
  avgconrate = data.frame (speeds, variable, conRateSpeeds)
  
  grate <- ggplot (avgconrate, aes(colour=variable)) +
    geom_line(aes (x=speeds, y=conRateSpeeds), size=1) +  
    ggtitle (sprintf("Consumer Mobility %s", opt$title)) +
    ylab ("Data Rate (Kbits/s)") +
    xlab ("Speed (m/s)") +
    scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
    scale_x_continuous (breaks=tbreak)
  
  outpng = sprintf("%s/%s-con-rate-vs-speed.png", opt$output, opt$scenario)
  
  png (outpng, width=1024, height=768)
  print (grate)
  x = dev.off ()
}

if (opt$producer)
{
  cat(sprintf ("Creating Average Data rate vs Speed for %s Producer %s mobility graph\n", opt$scenario, opt$e2))
  for (i in 1:length(strSpeeds))
  {
    file3n = sprintf("%s/%s-rate-trace-3n-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
    fileNdn = sprintf("%s/%s-rate-trace-smart-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
    
    prodRateSpeeds[i] = rateSummary(fileNdn, prodnodes)
    prodRateSpeeds[i+length(strSpeeds)] = rateSummary (file3n, prodnodes)
  }
  
  avgprodrate = data.frame (speeds, variable, prodRateSpeeds)
  
  gprod <- ggplot (avgprodrate, aes(colour=variable)) +
    geom_line(aes (x=speeds, y=prodRateSpeeds), size=1) +  
    ggtitle (sprintf("Consumer with Producer Mobility %s", opt$title)) +
    ylab ("Data Rate (Kbits/s)") +
    xlab ("Speed (m/s)") +
    scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
    scale_x_continuous (breaks=tbreak)
  
  outpng = sprintf("%s/%s-prod-rate-vs-speed.png", opt$output, opt$scenario)
  
  png (outpng, width=1024, height=768)
  print (gprod)
  x = dev.off ()
}
