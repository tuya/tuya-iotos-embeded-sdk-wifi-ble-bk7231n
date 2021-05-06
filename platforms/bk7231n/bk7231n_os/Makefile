OS := $(shell uname)


GIT_VERSION := $(shell git describe --dirty=-Dev)
.PHONY: mp
mp: 
	@$(MAKE) -f application.mk CFG_GIT_VERSION=\"$(GIT_VERSION)\"
	
.PHONY: beken
beken: 
	@$(MAKE) -f application.mk application

.PHONY: clean
clean:
	@$(MAKE) -f application.mk clean
	
.PHONY: flash debug ramdebug setup
setup:
	@$(MAKE) -f application.mk $(MAKECMDGOALS)

flash: toolchain
	@$(MAKE) -f application.mk flashburn
	
debug: toolchain
	@$(MAKE) -f application.mk debug	

ramdebug: toolchain
	@$(MAKE) -f application.mk ramdebug	
