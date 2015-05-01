#!/bin/bash

# Run 3N Consumer
echo "Running 3N Mobile Consumer"
./waf --run "nnn-icc-mobility --3n --trace"

# Run 3N Producer
echo "Running 3N Mobile Producer"
./waf --run "nnn-icc-mobility --3n --producer --trace"

# Run NDN Smart Flooding Consumer
echo "Running NDN Mobile Consumer"
./waf --run "nnn-icc-mobility --ndn --smart --trace"

# Run NDN Smart Flooding Producer
echo "Running NDN Mobile Producer"
./waf --run "nnn-icc-mobility --ndn --smart --producer --trace"

# Create comparison graphs
# App Delay
# Consumer Mobility
echo "Creating Consumer mobility network delay"
./graphs/app-data-compare.R -d -f results/MobilityICC-app-delays-smart-con-01-001-004.txt -c results/MobilityICC-app-delays-3n-con-01-001-004.txt --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Consumer" -s 20

# Producer Mobility
echo "Creating Producer mobility network delay"
./graphs/app-data-compare.R -d -f results/MobilityICC-app-delays-smart-prod-01-001-004.txt -c results/MobilityICC-app-delays-3n-prod-01-001-004.txt --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Producer" -s 20

# Aggregate packets
# Consumer Mobility
echo "Creating Consumer mobility Interest success rate"
./graphs/int-tr-compare.R -e 0 -f results/MobilityICC-aggregate-trace-smart-con-01-001-004.txt -c results/MobilityICC-aggregate-trace-3n-con-01-001-004.txt -t "Mobile consumer" --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -s 20

# Producer Mobility
echo "Creating Producer mobility Interest success rate"
./graphs/int-tr-compare.R -e 7 -f results/MobilityICC-aggregate-trace-smart-prod-01-001-004.txt -c results/MobilityICC-aggregate-trace-3n-prod-01-001-004.txt -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

# Special Producer Mobility graph
./graphs/app-data-j.R -s 20 -d -f results/MobilityICC-app-delays-3n-prod-01-001-004.txt -t "Application using 3N + Smart flooding"

# Data rate
# Consumer Mobility
echo "Mobile Consumer Data rate"
./graphs/rate-tr-icn-compare.R -e 0 -f results/MobilityICC-rate-trace-smart-con-01-001-004.txt -c results/MobilityICC-rate-trace-3n-con-01-001-004.txt -t "Mobile Consumer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

# Producer Mobility
echo "Consumer with Mobile Producer Data rate"
./graphs/rate-tr-icn-compare.R -e 7 -f results/MobilityICC-rate-trace-smart-prod-01-001-004.txt -c results/MobilityICC-rate-trace-3n-prod-01-001-004.txt -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"
