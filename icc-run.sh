#!/bin/bash

SPEEDS="1.40 2.80 4.20 5.60 7.00 8.40 9.80 11.20"
NSLOG=MobilityICC
NSscenario=nnn-icc-mobility
SCDE=01-001-004.txt

CONSUMER=0
PRODUCER=7

for i in $SPEEDS
do
    speed=${i//\./-}

    # Run 3N Consumer
    echo "Running 3N Mobile Consumer $i"
    ./waf --run "$NSscenario --3n --speed=$i --trace"

    # Run 3N Producer
    echo "Running 3N Mobile Producer $i"
    ./waf --run "$NSscenario --3n --speed=$i --producer --trace"

    # Run NDN Smart Flooding Consumer
    echo "Running NDN Mobile Consumer $i"
    ./waf --run "$NSscenario --ndn --smart --speed=$i --trace"

    # Run NDN Smart Flooding Producer
    echo "Running NDN Mobile Producer $i"
    ./waf --run "$NSscenario --ndn --smart --speed=$i --producer --trace"

    # Create comparison graphs
    # App Delay
    # Consumer Mobility
    echo "Creating Consumer mobility network delay $i"
    ./graphs/app-data-compare.R -r -d -f results/$NSLOG-app-delays-smart-con-$speed-$SCDE -c results/$NSLOG-app-delays-3n-con-$speed-$SCDE --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Consumer" -s 20

    # Producer Mobility
    echo "Creating Producer mobility network delay $i"
    ./graphs/app-data-compare.R -r -d -f results/$NSLOG-app-delays-smart-prod-$speed-$SCDE -c results/$NSLOG-app-delays-3n-prod-$speed-$SCDE --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -t "Mobile Producer" -s 20

    # Aggregate packets
    # Consumer Mobility
    echo "Creating Consumer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e $CONSUMER -f results/$NSLOG-aggregate-trace-smart-con-$speed-$SCDE -c results/$NSLOG-aggregate-trace-3n-con-$speed-$SCDE -t "Mobile consumer" --str1="NDN Smart Flooding" --str2="3N + Smart Flooding" -s 20

    # Producer Mobility
    echo "Creating Producer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e $PRODUCER -f results/$NSLOG-aggregate-trace-smart-prod-$speed-$SCDE -c results/$NSLOG-aggregate-trace-3n-prod-$speed-$SCDE -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

    # Special Producer Mobility graph
    ./graphs/app-data-j.R -r -s 20 -d -f results/$NSLOG-app-delays-3n-prod-$speed-$SCDE -t "Application using 3N + Smart Flooding"

    # Data rate
    # Consumer Mobility
    echo "Mobile Consumer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e $CONSUMER -f results/$NSLOG-rate-trace-smart-con-$speed-$SCDE -c results/$NSLOG-rate-trace-3n-con-$speed-$SCDE -t "Mobile Consumer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

    # Producer Mobility
    echo "Consumer with Mobile Producer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e $PRODUCER -f results/$NSLOG-rate-trace-smart-prod-$speed-$SCDE -c results/$NSLOG-rate-trace-3n-prod-$speed-$SCDE -t "Consumer with Mobile Producer" -s 20 --str1="NDN Smart Flooding" --str2="3N + Smart Flooding"

done
