# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_cluster_dma.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS 	+= -DASYNC
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
