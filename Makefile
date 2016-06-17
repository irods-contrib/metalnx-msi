MSIGET_ILLUMINA_META = msiget_illumina_meta
MSIPUT_POPULATE = msiobjput_populate
MSIPUT_JPEG_EXTRACT = msiobjjpeg_extract
MSIPUT_MDMANIFEST = msiobjput_mdmanifest
MSIPUT_MDBAM = msiobjput_mdbam
MSIPUT_MDVCF = msiobjput_mdvcf

MAKEFLAGS += --no-print-directory

LIBS = \
       $(MSIGET_ILLUMINA_META) \
       $(MSIPUT_POPULATE) \
       $(MSIPUT_JPEG_EXTRACT) \
       $(MSIPUT_MDMANIFEST) \
       $(MSIPUT_MDBAM) \
       $(MSIPUT_MDVCF)

.PHONY: all $(LIBS) clean
all: $(LIBS)

$(LIBS):
	@$(MAKE) -C $@;

clean:
	@$(MAKE) -C $(MSIGET_ILLUMINA_META) clean;
	@$(MAKE) -C $(MSIPUT_POPULATE) clean;
	@$(MAKE) -C $(MSIPUT_JPEG_EXTRACT) clean;
	@$(MAKE) -C $(MSIPUT_MDMANIFEST) clean;
	@$(MAKE) -C $(MSIPUT_MDBAM) clean;
	@$(MAKE) -C $(MSIPUT_MDVCF) clean;

