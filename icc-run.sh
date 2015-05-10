#!/bin/bash


SPEEDS="1.40 2.80 4.20 5.60 7.00 8.40 9.80 11.20"
NSLOG=MobilityICC
NSscenario=nnn-icc-mobility
SCDE=01-001-004.txt

CONSUMER=0
PRODUCER=7

T1="3n"
T2="smart"
STR1="NDN Smart Flooding"
STR2="3N + Smart Flooding"
DIR=results

###############################################################################
speedstr=""
for i in $SPEEDS
do
    speed=${i//\./-}

    # Run 3N Consumer
    echo "Running 3N Mobile Consumer $i"
    ./waf --run "$NSscenario --$T1 --speed=$i --trace"

    # Run 3N Producer
    echo "Running 3N Mobile Producer $i"
    ./waf --run "$NSscenario --$T1 --speed=$i --producer --trace"

    # Run NDN Smart Flooding Consumer
    echo "Running NDN Mobile Consumer $i"
    ./waf --run "$NSscenario --ndn --$T2 --speed=$i --trace"

    # Run NDN Smart Flooding Producer
    echo "Running NDN Mobile Producer $i"
    ./waf --run "$NSscenario --ndn --$T2 --speed=$i --producer --trace"

    # Create comparison graphs
    # App Delay
    # Consumer Mobility
    echo "Creating Consumer mobility network delay $i"
    ./graphs/app-data-compare.R -r -d -f $DIR/$NSLOG-app-delays-$T2-con-$speed-$SCDE -c $DIR/$NSLOG-app-delays-$T1-con-$speed-$SCDE --str1="$STR1" --str2="$STR2" -t "Mobile Consumer" -s 20

    # Producer Mobility
    echo "Creating Producer mobility network delay $i"
    ./graphs/app-data-compare.R -r -d -f $DIR/$NSLOG-app-delays-$T2-prod-$speed-$SCDE -c $DIR/$NSLOG-app-delays-$T1-prod-$speed-$SCDE --str1="$STR1" --str2="$STR2" -t "Mobile Producer" -s 20

    # Aggregate packets
    # Consumer Mobility
    echo "Creating Consumer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e $CONSUMER -f $DIR/$NSLOG-aggregate-trace-$T2-con-$speed-$SCDE -c $DIR/$NSLOG-aggregate-trace-$T1-con-$speed-$SCDE -t "Mobile consumer" --str1="$STR1" --str2="$STR2" -s 20

    # Producer Mobility
    echo "Creating Producer mobility Interest success rate $i"
    ./graphs/int-tr-compare.R -e $PRODUCER -f $DIR/$NSLOG-aggregate-trace-$T2-prod-$speed-$SCDE -c $DIR/$NSLOG-aggregate-trace-$T1-prod-$speed-$SCDE -t "Consumer with Mobile Producer" -s 20 --str1="$STR1" --str2="$STR2"

    # Special Producer Mobility graph
    ./graphs/app-data-j.R -r -s 20 -d -f $DIR/$NSLOG-app-delays-$T1-prod-$speed-$SCDE -t "Application using 3N + Smart Flooding"

    # Data rate
    # Consumer Mobility
    echo "Mobile Consumer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e $CONSUMER -f $DIR/$NSLOG-rate-trace-$T2-con-$speed-$SCDE -c $DIR/$NSLOG-rate-trace-$T1-con-$speed-$SCDE -t "Mobile Consumer" -s 20 --str1="$STR1" --str2="$STR2"

    # Producer Mobility
    echo "Consumer with Mobile Producer Data rate $i"
    ./graphs/rate-tr-icn-compare.R -e $PRODUCER -f $DIR/$NSLOG-rate-trace-$T2-prod-$speed-$SCDE -c $DIR/$NSLOG-rate-trace-$T1-prod-$speed-$SCDE -t "Consumer with Mobile Producer" -s 20 --str1="$STR1" --str2="$STR2"

done

speedstr=${SPEEDS// /,}

echo "Data rate vs Speed graphs"
./graphs/speed-avg-rate.R -s $NSLOG --str1="$STR1" --str2="$STR2" -m "$speedstr" --e1="$CONSUMER" --e2="$PRODUCER" -x $SCDE -d $DIR -p -c

echo "Max Seq and Delay vs Speed graphs"
./graphs/speed-avg-delay.R -s $NSLOG --str1="$STR1" --str2="$STR2" -m "$speedstr" -c -p --delay --maxSeq -x $SCDE -d $DIR
