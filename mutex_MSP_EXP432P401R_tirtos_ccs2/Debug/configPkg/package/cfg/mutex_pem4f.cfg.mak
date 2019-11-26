# invoke SourceDir generated makefile for mutex.pem4f
mutex.pem4f: .libraries,mutex.pem4f
.libraries,mutex.pem4f: package/cfg/mutex_pem4f.xdl
	$(MAKE) -f C:\Users\WILLIA~1\workspace_v9_2\mutex_MSP_EXP432P401R_tirtos_ccs2/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\WILLIA~1\workspace_v9_2\mutex_MSP_EXP432P401R_tirtos_ccs2/src/makefile.libs clean

