# User Test
#------------------------------------
W_M1 ?= 64
H_M1 ?= 64
W_M2 ?= 64
include MatMulModel.mk
include $(RULES_DIR)/at_common_decl.mk

io?=host

APP              = test
APP_SRCS        += MatMulRunTest.c $(MATMUL_SRC_CODE) $(MODEL_LIB_DSP)
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I$(TILER_DSP_KERNEL_PATH) -I$(MATMULBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC)
APP_CFLAGS 		+= -DPERF $(MATMUL_OPT_FLAG) #-DPRINTDEB #-DPRINT_INOUT

all:: gen_matmul_code

include $(RULES_DIR)/pmsis_rules.mk
