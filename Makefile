
CROSS_COMPILE ?= /opt/rasp-qt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf
SYSROOT ?= /opt/rasp-qt/2021-01-11-raspios-buster-armhf-lite-rootfs-qt-dev

export CROSS_COMPILE
export SYSROOT


.PHONY: all
all:
	@mkdir -p .build
	@cd .build && cmake -DCMAKE_TOOLCHAIN_FILE=../tool-arm-linux-gnueabihf.cmake ..
	@cd .build && make

.PHONY: clean
clean:
	@rm -rf .build
