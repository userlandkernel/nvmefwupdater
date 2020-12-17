all: clean arm64 x86_64

clean:
	rm -f nvmefwupdater_arm64 nvmefwupdater_x86_64

arm64:
	clang \
		-isysroot `xcode-select -p`/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS11.2.sdk \
		-arch arm64 \
		-O3 \
		main.c \
		-o nvmefwupdater_arm64

	ldid -Sentitlements.plist nvmefwupdater_arm64

x86_64:
	clang \
		-arch x86_64 \
		-O3 \
		main.c \
		-o nvmefwupdater_x86_64
