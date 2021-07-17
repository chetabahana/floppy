#
# Makefile for 'raread'
#
# Bob Eager   January 2000
#
# Product names
#
PRODUCT		= raread
#
# Compiler setup
#
CC		= icc
#
!IFDEF	PROD
CFLAGS		= -Fi -G4 -O -Q -Se -Si
!ELSE
CFLAGS		= -Fi -G4 -Q -Se -Si -Ti -Tm -Tx
!ENDIF
#
# Names of object files
#
OBJ =		$(PRODUCT).obj
#
# Other files
#
DEF =		$(PRODUCT).def
LNK =		$(PRODUCT).lnk
#
# Final executable file
#
EXE =		$(PRODUCT).exe
EXEB = 		$(PRODUCT)b.exe
#
#-----------------------------------------------------------------------------
#
$(EXE):		$(OBJ) $(LNK) $(DEF)
!IFDEF	PROD
		ilink /nologo /exepack:2 @$(LNK)
!ELSE
		ilink /debug /nobrowse /nologo @$(LNK)
!ENDIF
#
# Object files
#
raread.obj:	raread.c
#
# Linker response file. Rebuild if makefile changes
#
$(LNK):		makefile
		@if exist $(LNK) erase $(LNK)
		@echo /map:$(PRODUCT) >> $(LNK)
		@echo /out:$(PRODUCT) >> $(LNK)
		@echo $(OBJ) >> $(LNK)
		@echo $(DEF) >> $(LNK)
#
clean:		
		-erase $(OBJ) $(LNK) $(PRODUCT).map csetc.pch
#
release:	$(EXE) $(EXEB) readme.txt
		rm -f $(PRODUCT).zip
		zip -9 -j $(PRODUCT).zip readme.txt $(EXE) $(EXEB)
#
# End of makefile for 'raread'
#
