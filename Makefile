TOTK_VERSION ?= 121
#FTP_IP ?= 192.168.0.6 # upload mod to ftp server on your switch
.PHONY: all clean

all:
	cmake -DTOTK_VERSION=$(TOTK_VERSION) --toolchain=cmake/toolchain.cmake -S . -B build && $(MAKE) -C build subsdk9_meta -j8

send:
	cmake -DTOTK_VERSION=$(TOTK_VERSION) --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

# ryu manual eg:
# TOTK_VERSION=121 make && cp build/{main.npdm,subsdk9} ~/appdata/Roaming/Ryujinx/sdcard/atmosphere/contents/0100f2c0115b6000/exefs/

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0
