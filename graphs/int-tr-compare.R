#!/usr/bin/Rscript

# int-tr-compare.R
# Simple R script to make graphs from ndnSIM tracers - Compares Satisfied Interests / Timed out interests

#
# Copyright (c) 2014 Waseda University, Sato Laboratory
# Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
#
# int-tr-compare.R is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# int-tr-compare.R is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of              
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
# GNU Affero Public License for more details.                                 
#                                                                             
# You should have received a copy of the GNU Affero Public License            
# along with int-tr-compare.R.  If not, see <http://www.gnu.org/licenses/>.
#

suppressPackageStartupMessages(library (ggplot2))
suppressPackageStartupMessages(library (scales))
suppressPackageStartupMessages(library (optparse))
suppressPackageStartupMessages(library (doBy))
suppressPackageStartupMessages(library (plyr))

# set some reasonable defaults for the options that are needed
option_list <- list (
  make_option(c("-f", "--file"), type="character", default="results/aggregate.txt",
              help="File which holds the raw aggregate data.\n\t\t[Default \"%default\"]"),
  make_option(c("-c", "--compare"), type="character", default="",
              help="File which holds the raw app delay data for 3n.\n\t\t[Default \"%default\"]"),
  make_option(c("-e", "--node"), type="character", default="",
              help="Node data to graph. Can be a comma separated list.\n\t\tDefault graphs data for all nodes."),
  make_option(c("-o", "--output"), type="character", default=".",
              help="Output directory for graphs.\n\t\t[Default \"%default\"]"),
  make_option(c("-s", "--seq"), type="integer", default=10,
              help="Sets how apart the ticks are on graph.\n\t\t[Default \"%default\"]"),
  make_option(c("-t", "--title"), type="character", default="Scenario",
              help="Title for the graph"),
  make_option(c("--str1"), type="character", default="Smart Flooding",
              help="Legend title for -f file data\n\t\t[Default \"%default\"]"),
  make_option(c("--str2"), type="character", default="3N + Smart Flooding",
              help="Legend title for -c file data\n\t\t[Default \"%default\"]")
)

# Load the parser
opt = parse_args(OptionParser(option_list=option_list, description="Creates graphs from L3 Data Aggregate Tracer data"))

data = read.table (opt$file, header=T)
data$Node = factor (data$Node)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor (data$Type)

intdata = data

intdata = subset(intdata, Type %in% c("SatisfiedInterests", "TimedOutInterests"))

name = ""
filnodes = unlist(strsplit(opt$node, ","))

# Set the theme for graph output
theme_set(theme_grey(base_size = 24) + 
            theme(axis.text = element_text(colour = "black")))

# Get the basename of the file
tmpname = strsplit(opt$file, "/")[[1]]
filename = tmpname[length(tmpname)]
# Get rid of the extension
noext = gsub("\\..*", "", filename)

# Sequence for ticks

# Filter for a particular node
if (nchar(opt$node) > 0) {
  intdata = subset (intdata, Node %in% filnodes)
  
  if (dim(data)[1] == 0) {
    cat(sprintf("There is no Node %s in this trace!\n", opt$node))
    quit("yes")
  }
  
  intname = sprintf("%s Interest Success rate for Nodes %s", opt$title, opt$node)
  
  intdata.combined = summaryBy (. ~ Time + Node + Type, data=intdata, FUN=sum)
  
} else {
  intname = sprintf("%s Interest Success rate", opt$title)
  
  intdata.combined = summaryBy (. ~ Time + Type, data=intdata, FUN=sum)
}

intdata.combined$Variable = "1"

mobile1name = opt$str1

# Sequence for ticks
tbreak = seq (0, round (nrow (intdata.combined), -1), opt$seq)

if (nchar(opt$compare) > 0)
{
  dataC = read.table (opt$compare, header=T)
  dataC$Node = factor (dataC$Node)
  dataC$Kilobits <- dataC$Kilobytes * 8
  dataC$Type = factor (dataC$Type)
  
  dataC = subset (dataC, Type %in% c("SatisfiedInterests", "TimedOutInterests"))
  
  if (nchar(opt$node) > 0) {
    dataC = subset (dataC, Node %in% filnodes)
    
    dataC.combined = summaryBy (. ~ Time + Node + Type, data=dataC, FUN=sum)
  } else {
    dataC.combined = summaryBy (. ~ Time + Type, data=dataC, FUN=sum)
  }
  
  dataC.combined$Variable = "2"
  
  mobile2name = opt$str2
  
  combinedlist = list(intdata.combined, dataC.combined)
  allcombineddata = do.call(rbind.fill, combinedlist)
}

if (nchar(opt$node) > 0) {
  if (nchar(opt$compare) > 0)
  {
    cat ("Creating Interest rate comparison graph for nodes\n")
    g.int <- ggplot (allcombineddata, aes(linetype=Type, colour=Variable)) +
      geom_line(aes (x=Time, y=Packets.sum), size=0.5) +  
      ggtitle (intname) +
      ylab ("Packets / Seconds") +
      xlab ("Simulation time (Seconds)") +
      facet_wrap (~ Node) +
      scale_colour_discrete(name = "Strategies", labels = c(mobile1name, mobile2name)) +
      scale_x_continuous (breaks=tbreak)
  } else {
    cat ("Creating Interest rate graph for nodes\n")
    g.int <- ggplot (intdata.combined, aes(x=Time, y=Packets.sum, linetype=Type)) +
      geom_line(aes (linetype=Type), size=0.5) + 
      geom_point(aes (shape=Type), size=1) +  
      ggtitle (intname) +
      ylab ("Packets / Seconds") +
      xlab ("Simulation time (Seconds)") +
      facet_wrap (~ Node) +
      scale_x_continuous (breaks=tbreak)
  }
} else {
  if (nchar(opt$compare) > 0)
  {
    cat ("Creating Interest rate comparison graph\n")
    g.int <- ggplot (allcombineddata, aes (linetype=Type, colour=Variable)) +
      geom_line(aes (x=Time, y=Packets.sum), size=0.5) +  
      ggtitle (intname) +
      ylab ("Packets / Seconds") +
      xlab ("Simulation time (Seconds)") +
      scale_colour_discrete(name = "Strategies", labels = c(mobile1name, mobile2name)) +
      scale_x_continuous (breaks=tbreak)
  } else {
    cat ("Creating Interest rate graph\n")
    g.int <- ggplot (intdata.combined, aes(linetype=Type)) +
      geom_line(aes (x=Time, y=Packets.sum), size=0.5) +  
      ggtitle (intname) +
      ylab ("Packets / Seconds") +
      xlab ("Simulation time (Seconds)") +
      scale_x_continuous (breaks=tbreak)
  }
}

outInpng = ""
# The output png
if (nchar(opt$node) > 0) {
  if (nchar (opt$compare) > 0)
  {
    outInpng = sprintf("%s/%s-%s-comp-sin.png", opt$output, noext, paste(filnodes, sep="", collapse="_"))
  } else {
    outInpng = sprintf("%s/%s-%s-sin.png", opt$output, noext, paste(filnodes, sep="", collapse="_"))
  }
} else {
  if (nchar(opt$compare) > 0)
  {
    outInpng = sprintf("%s/%s-comp-sin.png", opt$output, noext)
  } else {
    outInpng = sprintf("%s/%s-sin.png", opt$output, noext)
  }
}

png (outInpng, width=1024, height=768)
print (g.int)
x = dev.off ()