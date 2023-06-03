DTC = dtc
OVERLAY_NAME = st7735-overlay
DIR = ../.out

all: $(DIR)/$(OVERLAY_NAME).dtbo

$(DIR)/$(OVERLAY_NAME).dtbo: $(OVERLAY_NAME).dts
	mkdir -p $(DIR) && $(DTC) -I dts -O dtb -o $@ $<

clean:
	rm -f $(DIR)/$(OVERLAY_NAME).dtbo