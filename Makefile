prepareios:
	cd ios && pod deintegrate && pod install && cd ..
	npx expo prebuild

runios:
	make prepareios
	npx expo run:ios --device d548d4589b4e42ae363a969e1f8a161e37c5af15