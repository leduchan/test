# User Test
#------------------------------------

APP              = test
APP_SRCS         = test_dmacpy.c
APP_INC          =
APP_CFLAGS       =
APP_LDFLAGS      =

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif				# ASYNC

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
