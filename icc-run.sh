#!/bin/bash

SPEED="1.40 2.80 5.60 11.20"

for i in $SPEED
do
    FNAME=${i//\./-}

    # Run 3N Consumer
    echo "Running 3N Mobile Consumer $i"
    ./waf --run "nnn-icc-mobility --3n --speed=$i --trace"

    # Run 3N Producer
    echo "Running 3N Mobile Producer $i"
    ./waf --run "nnn-icc-mobility --3n --speed=$i --producer --trace"

    # Run NDN Smart Flooding Consumer
    echo "Running NDN Mobile Consumer $i"
    ./waf --run "nnn-icc-mobility --ndn --smart --speed=$i --trace"

    # Run NDN Smart Flooding Producer
    echo "Running NDN Mobile Producer $i"
    ./waf --run "nnn-icc-mobility --ndn --smart --speed=$i --producer --trace"

    # Create comparison graphs
    # App Delay
    # Consumer Mobility
    echo "Creating Consumer mobility network delay $i"
    ./graphs/app-data-compare.R -d -f results/MobilityICC-app-delays-smart-con-$FNAME-01-001-004.txt -c results/MobilityICC-app-delays-3n-con-$FNAME-01-001-004.txt --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Consumer" -s 20

    # Producer Mobility
    echo "Creating Producer mobility network delay $i"
    ./graphs/app-data-compare.R -d -f results/MobilityICC-app-delays-smart-prod-$FNAME-01-001-004.txt -c results/MobilityICC-app-delays-3n-prod-$FNAME-01-001-004.txt --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Producer" -s 20

    # Aggregate packets
    # Consumer Mobility
    echo "Creating Consumer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e 0 -f results/MobilityICC-aggregate-trace-smart-con-$FNAME-01-001-004.txt -c results/MobilityICC-aggregate-trace-3n-con-$FNAME-01-001-004.txt -t "Mobile consumer" --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -s 20

    # Producer Mobility
    echo "Creating Producer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e 7 -f results/MobilityICC-aggregate-trace-smart-prod-$FNAME-01-001-004.txt -c results/MobilityICC-aggregate-trace-3n-prod-$FNAME-01-001-004.txt -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

    # Special Producer Mobility graph
    ./graphs/app-data-j.R -s 20 -d -f results/MobilityICC-app-delays-3n-prod-$FNAME-01-001-004.txt -t "Application using 3N + Smart Flooding"

    # Data rate
    # Consumer Mobility
    echo "Mobile Consumer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e 0 -f results/MobilityICC-rate-trace-smart-con-$FNAME-01-001-004.txt -c results/MobilityICC-rate-trace-3n-con-$FNAME-01-001-004.txt -t "Mobile Consumer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

    # Producer Mobility
    echo "Consumer with Mobile Producer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e 7 -f results/MobilityICC-rate-trace-smart-prod-$FNAME-01-001-004.txt -c results/MobilityICC-rate-trace-3n-prod-$FNAME-01-001-004.txt -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

done
