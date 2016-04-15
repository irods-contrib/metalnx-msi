MSIGET_ILLUMINA_META = msiget_illumina_meta
MSIPUT_POPULATE = msiobjput_populate

MAKEFLAGS += --no-print-directory

LIBS = \
       $(MSIGET_ILLUMINA_META) \
	   $(MSIPUT_POPULATE)

.PHONY: all $(LIBS) clean
all: $(LIBS)

$(LIBS):
	@$(MAKE) -C $@;

clean:
	@$(MAKE) -C $(MSIGET_ILLUMINA_META) clean;
	@$(MAKE) -C $(MSIPUT_POPULATE) clean;


