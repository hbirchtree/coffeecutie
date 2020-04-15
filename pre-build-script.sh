#
# Download a downgraded Android build image, uses NDK 14 instead of 16
#
if [[ "$BUILDVARIANT" == "android."* ]]; then
    docker pull hbirch/android:r21
    docker tag hbirch/android:r21 hbirch/coffeecutie:android
fi
