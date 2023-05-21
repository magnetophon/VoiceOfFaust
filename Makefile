SHELL := /usr/bin/env bash

STANDALONES := $(wildcard standalones/*.dsp)
FOLLOWERS := $(wildcard followers/*.dsp)
FOLLOWERS_LV2 := $(FOLLOWERS:%.dsp=%.lv2)

DESTDIR ?=
PREFIX ?= /usr/local
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
LIBDIR ?= $(DESTDIR)$(PREFIX)/lib
# put th PD patch in the bin dir, so the binaries can find them using a relative path
PUREDATADIR ?= $(BINDIR)/PureData

LV2_URI_PREFIX := https://magnetophon.nl

.PHONY: all standalones followers lv2 install-standalones install-followers install-lv2 clean

all: standalones followers lv2

standalones: $(STANDALONES:%.dsp=%.standalone)

%.standalone: %.dsp
	echo "compiling jack standalone from $<"
	faust2jack -preset auto -t -1 -time -osc -vec $<

followers: $(FOLLOWERS:%.dsp=%.follower)

%.follower: %.dsp
	echo "compiling jack follower from $<"
	faust2jack -preset auto -t -1 -time -osc -vec $<

lv2: $(FOLLOWERS_LV2)

%.lv2: %.dsp
	echo "compiling lv2 from: $<"
	faust2lv2 -gui -t -1 -time -osc -vec -uri-prefix $(LV2_URI_PREFIX) $<

install-standalones:
	install -d $(BINDIR)
	$(foreach f, $(basename $(STANDALONES)), install -m 755 standalones/$(notdir $(f)) $(BINDIR)/$(notdir $(f));)
	install -m 755 launchers/* $(BINDIR)
	install -m 644 launchers/synthWrapper $(BINDIR)/synthWrapper
	install -d $(PUREDATADIR)
	install -m 644 PureData/* $(PUREDATADIR)

install-followers:
	install -d $(BINDIR)
	$(foreach f, $(basename $(FOLLOWERS)), install -m 755 followers/$(notdir $(f)) $(BINDIR)/$(notdir $(f));)

install-lv2:
	install -d $(LIBDIR)/lv2
	$(foreach p, $(wildcard followers/*.lv2), cp -rfd $(p) $(LIBDIR)/lv2;)

install: install-followers install-standalones install-lv2

clean:
	rm -rf $(basename $(STANDALONES)) $(basename $(FOLLOWERS)) $(FOLLOWERS_LV2)
