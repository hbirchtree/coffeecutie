.PRECIOUS: icon.svg icon_circle.svg icon_rounded.svg icon_squircle.svg

# TODO: Export as plain SVGs, not Inkscape SVGs

icon.svg: components/background.svg components/foreground.svg svg_combine.py
	./svg_combine.py components/background.svg components/foreground.svg > $@

icon_%.svg: work/masked_%.svg
	inkscape --batch-process --actions='select-all;ObjectSetClipPath;select-all;export-filename:$@;export-do' work/masked_$*.svg

icon_circle_%.png: icon_circle.svg
	inkscape $< \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

icon_rounded_%.png: icon_circle.svg
	inkscape $< \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

icon_squircle_%.png: icon_squircle.svg
	inkscape $< \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*
	
clean:
	rm -rf work/
	
work:
	mkdir -p work/
	
work/masked_%.svg: masks/%.svg icon.svg svg_combine.py work
	./svg_combine.py icon.svg masks/$*.svg > $@

work/circle_%.png: masks/circle.svg
	inkscape masks/circle.svg \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

work/rounded_%.png: masks/rounded.svg
	inkscape masks/rounded.svg \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

work/squircle_%.png: masks/squircle.svg
	inkscape masks/squircle.svg \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

work/icon_%.png: icon.svg
	inkscape icon.svg \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

work/foreground_%.png: components/foreground.svg
	inkscape components/foreground.svg \
		--export-filename=$@ \
		--export-width=$* \
		--export-height=$*

work/splash_$(WIDTH)_$(HEIGHT).png: components/splash.svg work
	inkscape components/splash.svg \
		--export-filename=$@ \
		--export-width=$(WIDTH) \
		--export-height=$(HEIGHT)

work/mipmap_%.png: work/icon_$(SIZE).png work/circle_$(SIZE).png work/rounded_$(SIZE).png work/squircle_$(SIZE).png work/foreground_$(SIZE).png work
	cp work/icon_$(SIZE).png work/mipmap_$*.png
	cp work/foreground_$(SIZE).png work/foreground_$*.png
	cp work/circle_$(SIZE).png work/circle_$*.png
	cp work/rounded_$(SIZE).png work/rounded_$*.png
	cp work/squircle_$(SIZE).png work/squircle_$*.png

android:
	ln -s ../android/Gradle/app/src/main/res android

android/mipmap-%/ic_launcher.png: work/mipmap_%.png
	convert \
		work/mipmap_$*.png \
		work/squircle_$*.png \
		-compose copy-opacity \
		-composite \
		$@

android/mipmap-%/ic_launcher_round.png: work/mipmap_%.png
	convert \
		work/mipmap_$*.png \
		work/circle_$*.png \
		-compose copy-opacity \
		-composite \
		$@

android/mipmap-%/ic_launcher_foreground.png: work/mipmap_%.png
	cp work/foreground_$*.png $@

android-icons: android
	make android/mipmap-mdpi/ic_launcher.png android/mipmap-mdpi/ic_launcher_round.png android/mipmap-mdpi/ic_launcher_foreground.png -e SIZE=48
	make android/mipmap-hdpi/ic_launcher.png android/mipmap-hdpi/ic_launcher_round.png android/mipmap-hdpi/ic_launcher_foreground.png -e SIZE=72
	make android/mipmap-xhdpi/ic_launcher.png android/mipmap-xhdpi/ic_launcher_round.png android/mipmap-xhdpi/ic_launcher_foreground.png -e SIZE=96
	make android/mipmap-xxhdpi/ic_launcher.png android/mipmap-xxhdpi/ic_launcher_round.png android/mipmap-xxhdpi/ic_launcher_foreground.png -e SIZE=144
	make android/mipmap-xxxhdpi/ic_launcher.png android/mipmap-xxxhdpi/ic_launcher_round.png android/mipmap-xxxhdpi/ic_launcher_foreground.png -e SIZE=192
	echo Generated Android icons

ios/icons/icon_large.png: work/mipmap_large.png
	convert \
		work/mipmap_large.png \
		work/squircle_large.png \
		-compose copy-opacity \
		-composite \
		$@

ios/splash/Default-Landscape-%h@$(SCALE)x~$(TYPE).png: work/splash_$(WIDTH)_$(HEIGHT).png work/foreground_$(ICON_SIZE).png
	convert \
		work/splash_$(WIDTH)_$(HEIGHT).png \
		work/foreground_$(ICON_SIZE).png \
		-gravity center \
		-composite \
		ios/splash/Default-Landscape-$*h@$(SCALE)x~$(TYPE).png

ios-icons:
	make ios/icons/icon_large.png -e SIZE=512
	echo "Generating iPhoneOS launch images"
	make ios/splash/Default-Landscape-480h@1x~iphone.png -e WIDTH=480 -e HEIGHT=320 -e SCALE=1 -e TYPE=iphone -e ICON_SIZE=128
	make ios/splash/Default-Landscape-960h@2x~iphone.png -e WIDTH=960 -e HEIGHT=640 -e SCALE=2 -e TYPE=iphone -e ICON_SIZE=320
	make ios/splash/Default-Landscape-1136h@2x~iphone.png -e WIDTH=1136 -e HEIGHT=640 -e SCALE=2 -e TYPE=iphone -e ICON_SIZE=320
	make ios/splash/Default-Landscape-1334h@2x~iphone.png -e WIDTH=1334 -e HEIGHT=750 -e SCALE=2 -e TYPE=iphone -e ICON_SIZE=384
	make ios/splash/Default-Landscape-2208h@3x~iphone.png -e WIDTH=2208 -e HEIGHT=1242 -e SCALE=3 -e TYPE=iphone -e ICON_SIZE=512
	make ios/splash/Default-Landscape-2436h@3x~iphone.png -e WIDTH=2436 -e HEIGHT=1125 -e SCALE=3 -e TYPE=iphone -e ICON_SIZE=512
	echo "Generating iPadOS launch images"
	make ios/splash/Default-Landscape-1024h@1x~ipad.png -e WIDTH=1024 -e HEIGHT=768 -e SCALE=1 -e TYPE=ipad -e ICON_SIZE=192
	make ios/splash/Default-Landscape-2048h@2x~ipad.png -e WIDTH=2048 -e HEIGHT=1536 -e SCALE=2 -e TYPE=ipad -e ICON_SIZE=384
	echo "Generating tvOS launch images"
	make ios/splash/Default-Landscape-1280h@1x~tv.png -e WIDTH=1280 -e HEIGHT=720 -e SCALE=1 -e TYPE=tv -e ICON_SIZE=256
	make ios/splash/Default-Landscape-1920h@1x~tv.png -e WIDTH=1920 -e HEIGHT=1080 -e SCALE=1 -e TYPE=tv -e ICON_SIZE=384
	make ios/splash/Default-Landscape-3840h@2x~tv.png -e WIDTH=3840 -e HEIGHT=2160 -e SCALE=2 -e TYPE=tv -e ICON_SIZE=768

