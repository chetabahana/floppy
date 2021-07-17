#
# Makefile for V2GB.SYS
#
# Bob Eager   February 2003
#
# Product name
#
PRODUCT		= v2gb
#
# Target directory
#
TARGET		= g:\os2\boot
#
# Compiler setup
#
CC		= icc
!IFDEF	DEBUG
CFLAGS		= -DDEBUG -Fi -Gr+ -Gs -O -Q -Rn -Se -Si -Sm -Ss -Tl30 -W2
!ELSE
CFLAGS		= -Fi -Gr+ -Gs -O -Q -Rn -Se -Si -Sm -Ss -Tl30 -W2
!ENDIF
#
# Names of object files
#
OBJS =		v2gbinit.obj v2gbuser.obj v2gbint.obj v2gbdata.obj
LIBS =		vdh.lib
#
# Other files
#
DEF =		$(PRODUCT).def
LNK =		$(PRODUCT).lnk
#
# Final loadable file
#
SYS =		$(PRODUCT).sys
#
# Release file
#
ZIP = 		$(PRODUCT).zip
#
#-----------------------------------------------------------------------------
#
install:	$(SYS)
		@copy $(SYS) $(TARGET) > nul
#
$(SYS):		$(OBJS) $(LNK) $(DEF)
		ilink /nologo /align:16 @$(LNK)
		@mv $(PRODUCT).vdd $(SYS)
#
# Object files
#
v2gbinit.obj:	v2gbinit.c v2gb.h
#
v2gbuser.obj:	v2gbuser.c v2gb.h
#
v2gbint.obj:	v2gbint.c v2gb.h
#
v2gbdata.obj:	v2gbdata.c v2gb.h
#
# Linker response file. Rebuild if makefile changes
#
$(LNK):		makefile
		@if exist $(LNK) erase $(LNK)
		@echo /map:$(PRODUCT) >> $(LNK)
		@echo /out:$(PRODUCT) >> $(LNK)
		@echo $(OBJS) >> $(LNK)
		@echo $(LIBS) >> $(LNK)
		@echo $(DEF) >> $(LNK)
#
clean:		
		-erase $(OBJS) $(LNK) $(PRODUCT).map csetc.pch
#
release:	$(SYS) readme.txt
		@rm -f $(ZIP)
		@zip -9 $(ZIP) $(SYS) readme.txt
#
# End of makefile for V2GB.SYS
#
