all: make_smbd install
clean: clean_smbd
force: clean all

RUN_DIR ?= /var/samba

ifeq ($(INSTALL_DIR),)
    CURR_DIR := $(shell pwd)
    BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
    BUILD_DIR:=$(word 1, $(BUILD_DIR))
    include $(BUILD_DIR)/make.common
endif

make_smbd: source/Makefile
	@$(MAKE) -C ./source srcdir=./ builddir=./

install:
	@install -m 755 ./source/bin/smbd $(INSTALL_DIR)/bin/smbd
	@install -m 755 ./source/bin/smbpasswd $(INSTALL_DIR)/bin/smbpasswd
#	@install -m 444 ./source/conf/smb.conf $(INSTALL_DIR)/etc
#	@install -m 755 ./source/bin/libbigballofmud.so $(INSTALL_DIR)/lib/libbigballofmud.so.0

source/Makefile:
	cd ./source; ./configure --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX) --disable-developer --enable-cups=no --enable-fam=no --enable-shared=yes --enable-iprint=no --enable-pie=no --without-ldap --without-cifsupcall --without-cifsmount --without-cluster-support --without-acl-support --with-privatedir=$(RUN_DIR) --with-lockdir=$(RUN_DIR)/locks --with-configdir=$(RUN_DIR) --with-piddir=$(RUN_DIR) --with-logfilebase=$(RUN_DIR) --with-krb5=no --disable-krb5developer --disable-developer --disable-debug --without-ads --without-winbind --without-quotas --disable-dmalloc --without-sys-quotas --without-utmp --with-included-popt=no --without-sendfile-support --with-libmsrpc=no --with-winbind=no --with-syslog=no SMB_BUILD_CC_NEGATIVE_ENUM_VALUES=no linux_getgrouplist_ok=no samba_cv_have_longlong=yes samba_cv_HAVE_IFACE_IFCONF=yes --enable-small-build

clean_smbd:
	@if [ -f ./source/Makefile ];then $(MAKE) -C ./source clean srcdir=./ builddir=./;fi;
	@rm -Rf ./source/bin/libbigballofmud.so $(INSTALL_DIR)/lib/libbigballofmud.so.0
	@rm -Rf  $(INSTALL_DIR)/bin/smbd $(INSTALL_DIR)/bin/smbpasswd
	@rm -f ./source/Makefile

