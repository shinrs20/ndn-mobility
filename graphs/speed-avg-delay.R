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

# set some reasonable defaults for the options that are needed
option_list <- list (
  make_option(c("-d", "--directory"), type="character", default="~/git/ndn-mobility/results",
              help="Directory which holds the raw data.\n\t\t[Default \"%default\"]"),
  make_option(c("-o", "--output"), type="character", default=".",
              help="Output directory for graphs.\n\t\t[Default \"%default\"]"),
  make_option(c("--e1"), type="character", default="",
              help="Consumer node data to graph. Can be a comma separated list.\n\t\tDefault graphs data for all nodes."),
  make_option(c("--e2"), type="character", default="",
              help="Producer node data to graph. Can be a comma separated list.\n\t\tDefault graphs data for all nodes."),
  make_option(c("-m", "--speeds"), type="character", default="1.4,2.8,4.2,5.6,7,8.4,9.8,11.2",
              help="Mobile speeds to compare.\n\t\t[Default \"%default\"]"),
  make_option(c("--str1"), type="character", default="NDN Smart Flooding",
              help="Legend title for -f file data\n\t\t[Default \"%default\"]"),
  make_option(c("--str2"), type="character", default="3N + Smart Flooding",
              help="Legend title for -c file data\n\t\t[Default \"%default\"]"),
  make_option(c("-c", "--consumer"), action="store_true", default=FALSE,
              help="Tell the script to graph Consumer mobility data"),
  make_option(c("-p", "--producer"), action="store_true", default=FALSE,
              help="Tell the script to graph Producer mobility data"),
  make_option(c("--delay"), action="store_true", default=FALSE,
              help="Tell the script to graph Delay data"),
  make_option(c("--maxSeq"), action="store_true", default=FALSE,
              help="Tell the script to graph Max sequence seen data"),
  make_option(c("-s", "--scenario"), type="character", default="MobilityICC",
              help="Name of the scenario to produce graphs for.\n\t\t[Default \"%default\"]"),
  make_option(c("-x", "--extension"), type="character", default="01-001-004.txt",
              help="The subfix of the extension files to use.\n\t\t[Default \"%default\"]")
)

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

# Load the parser
opt = parse_args(OptionParser(option_list=option_list, description="Creates summary App graphs for 3N vs NDN"))

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
conDelaySpeeds = double (length(speeds))
prodDelaySpeeds = double(length(speeds))

conSeqSpeeds = double (length(speeds))
prodSeqSpeeds = double(length(speeds))

theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

if (opt$consumer)
{
  if (opt$delay)
  {
    cat(sprintf ("Creating Average Delay vs Speed for %s Consumer %s mobility graph\n", opt$scenario, opt$e1))
    for (i in 1:length(strSpeeds))
    {
      file3n = sprintf("%s/%s-app-delays-3n-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      fileNdn = sprintf("%s/%s-app-delays-smart-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      
      conDelaySpeeds[i] = speedSummary (fileNdn, connodes)
      conDelaySpeeds[i+length(strSpeeds)] = speedSummary (file3n, connodes)
    }
    
    avgcondelay = data.frame (speeds, variable, conDelaySpeeds)
    
    gcondelay <- ggplot (avgcondelay, aes(colour=variable)) +
      geom_line(aes (x=speeds, y=conDelaySpeeds), size=1) +  
      ggtitle ("Consumer Mobility Avg Network Delay vs Speed") +
      ylab ("Delay [Seconds]") +
      xlab ("Speed (m/s)") +
      scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
      scale_x_continuous (breaks=tbreak)
    
    outpng = sprintf("%s/%s-con-delay-vs-speed.png", opt$output, opt$scenario)
    
    png (outpng, width=1024, height=768)
    print (gcondelay)
    x = dev.off ()
  }
  
  if (opt$maxSeq)
  {
    for (i in 1:length(strSpeeds))
    {
      file3n = sprintf("%s/%s-app-delays-3n-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      fileNdn = sprintf("%s/%s-app-delays-smart-con-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      
      conSeqSpeeds[i] = seqSummary (fileNdn, connodes)
      conSeqSpeeds[i+length(strSpeeds)] = seqSummary (file3n, connodes)
    }
    
    cat(sprintf ("Creating Average Max Sequence vs Speed for %s Consumer %s mobility graph\n", opt$scenario, opt$e1))
    
    avgconSeq = data.frame (speeds, variable, conSeqSpeeds)
    
    gconSeq <- ggplot (avgconSeq, aes(x=speeds, y=conSeqSpeeds)) +
      geom_line(aes (colour=variable), size=1) +
      ggtitle ("Consumer Mobility Received Sequence vs Speed") +
      ylab ("Sequence Number Received") +
      xlab ("Speed (m/s)") +
      scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
      scale_x_continuous (breaks=tbreak)
    
    outpng = sprintf("%s/%s-con-seq-vs-speed.png", opt$output, opt$scenario)
    
    png (outpng, width=1024, height=768)
    print (gconSeq)
    x = dev.off ()
  }
}

if (opt$producer)
{
  if (opt$delay)
  {
    cat(sprintf ("Creating Average Delay vs Speed for %s Producer %s mobility graph\n", opt$scenario, opt$e2))
    for (i in 1:length(strSpeeds))
    {
      file3n = sprintf("%s/%s-app-delays-3n-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      fileNdn = sprintf("%s/%s-app-delays-smart-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      
      prodDelaySpeeds[i] = speedSummary(fileNdn, prodnodes)
      prodDelaySpeeds[i+length(strSpeeds)] = speedSummary (file3n, prodnodes)
    }
    
    avgproddelay = data.frame (speeds, variable, prodDelaySpeeds)
    
    gproddelay <- ggplot (avgproddelay, aes(colour=variable)) +
      geom_line(aes (x=speeds, y=prodDelaySpeeds), size=1) +  
      ggtitle ("Consumer with Producer Mobility Avg Network Delay vs Speed") +
      ylab ("Delay [Seconds]") +
      xlab ("Speed (m/s)") +
      scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
      scale_x_continuous (breaks=tbreak)
    
    outpng = sprintf("%s/%s-prod-delay-vs-speed.png", opt$output, opt$scenario)
    
    png (outpng, width=1024, height=768)
    print (gproddelay)
    x = dev.off ()
  }
  
  if (opt$maxSeq)
  {
    cat(sprintf ("Creating Average Max Sequence vs Speed for %s Producer %s mobility graph\n", opt$scenario, opt$e1))
    for (i in 1:length(strSpeeds))
    {
      file3n = sprintf("%s/%s-app-delays-3n-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      fileNdn = sprintf("%s/%s-app-delays-smart-prod-%s-%s", opt$directory, opt$scenario, strSpeeds[i], opt$extension)
      
      prodSeqSpeeds[i] = seqSummary (fileNdn, connodes)
      prodSeqSpeeds[i+length(strSpeeds)] = seqSummary (file3n, connodes)
    }
    
    avgprodSeq = data.frame (speeds, variable, prodSeqSpeeds)
    
    gprodseq <- ggplot (avgprodSeq, aes(x=speeds, y=prodSeqSpeeds)) +
      geom_line(aes (colour=variable), size=1) +  
      ggtitle ("Consumer with Producer Mobility Received Sequence vs Speed") +
      ylab ("Sequence Number Received") +
      xlab ("Speed (m/s)") +
      scale_colour_discrete(name = "Strategies", labels = c(opt$str1, opt$str2)) +
      scale_x_continuous (breaks=tbreak)
    
    outpng = sprintf("%s/%s-prod-seq-vs-speed.png", opt$output, opt$scenario)
    
    png (outpng, width=1024, height=768)
    print (gprodseq)
    x = dev.off ()
  }
}
