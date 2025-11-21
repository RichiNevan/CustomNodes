prepareios:
	cd ios && pod deintegrate && pod install && cd ..
	npx expo prebuild

runios:
	make prepareios
	npx expo run:ios --device

runandroid:
	npx expo run:android --device