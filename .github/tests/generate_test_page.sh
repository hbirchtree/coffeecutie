#!/bin/bash

ARTIFACT_DIR=${1:-artifacts/}

TEST_VARIANTS=$(ls $ARTIFACT_DIR)

mkdir $ARTIFACT_DIR/generated

OUT_FILE=$ARTIFACT_DIR/generated/results.html

echo '<html>
<head>
    <title>Test results!</title>
</head>
<body style="background-color: black; color: white;">' > $OUT_FILE

for TEST_VARIANT in $TEST_VARIANTS; do
    echo $ARTIFACT_DIR/$TEST_VARIANT
    echo "<h1>$TEST_VARIANT</h1>" >> $OUT_FILE
    for IMG in $(find $ARTIFACT_DIR/$TEST_VARIANT/ -name *.jpg); do
        echo "<h3>$(basename $IMG)</h3>" >> $OUT_FILE
        echo "<img src=\"data:image/jpeg;base64,$(base64 --wrap=0 $IMG)\"/>" >> $OUT_FILE
    done
    for LOG in $(find $ARTIFACT_DIR/$TEST_VARIANT/ -name *.log); do
        echo "<h3>$(basename $LOG)</h3>" >> $OUT_FILE
        echo "<textarea style=\"background-color: black; color: white;\" rows=\"20\" cols=\"80\">$(cat $LOG)</textarea>" >> $OUT_FILE
    done
done

echo '</body>
</html>' >> $OUT_FILE
