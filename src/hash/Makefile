VCPATH=C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC
SDKPATH=C:\Program Files\Microsoft SDKs\Windows\v6.0A 
INCLUDEPATH=.

BIN_DIR=..\bin

LINKCMD = link
LINKFLAGS_DEBUG = /nologo  /MACHINE:X64 \
  /libpath:"$(VCPATH)\lib\amd64" /libpath:"$(SDKPATH)\Lib\x64" \
  /libpath:"." /libpath:"$(BIN_DIR)" \
  /DEBUG \
  /INCREMENTAL:NO \
  /OUT:"$(BIN_DIR)\$@" 
  
LINKFLAGS_RELEASE = /nologo  /MACHINE:X64 \
  /libpath:"$(VCPATH)\lib\amd64" /libpath:"$(SDKPATH)\Lib\x64" \
  /libpath:"." /libpath:"$(BIN_DIR)" \
  /DEBUG \
  /INCREMENTAL:NO \
  /OUT:"$(BIN_DIR)\$@" 
  
    
CLFLAGS_RELEASE = /I"$(INCLUDEPATH)" \
         /DNDEBUG /D "WIN32" \
         /D_WIN32_WINNT=0x0400 \
         /nologo /W3 /WX- /MD \
         /Zi /O2 /Oi /Oy- /EHsc /Gd \
         /GS /Fo"$(BIN_DIR)\\"

CLFLAGS_DEBUG = /I"$(INCLUDEPATH)" \
          /D "_DEBUG" /D "WIN32" \
          /nologo /W3 /WX- /MDd \
          /ZI /Gm /Od /Oi /Oy- /EHsc /Gd \
          /Fo"$(BIN_DIR)\\"


BUILD_TYPE = RELEASE

!if "$(BUILD_TYPE)" == "RELEASE"
!message Making a release build......
LINKFLAGS = $(LINKFLAGS_RELEASE)
CLFLAGS = $(CLFLAGS_RELEASE)
!elseif "$(BUILD_TYPE)" == "DEBUG"
!message Making a debug build......
LINKFLAGS = $(LINKFLAGS_DEBUG)
CLFLAGS = $(CLFLAGS_DEBUG)
!else
!message Invalid build type "$(BUILD_TYPE)" specified.
!endif
                  
CL = cl

EXES= hash_test.exe

OBJS= $(BIN_DIR)\GeneralHashFunctions.obj


#================================================================
# Suffix rules
#================================================================
.SUFFIXES :
.SUFFIXES : .cpp .cc .c .obj .exe

.c.obj :
	$(CL) /c $(CLFLAGS) $<

.cc.obj :
	$(CL) /c $(CLFLAGS) $<

.cpp.obj :
	$(CL) /c $(CLFLAGS) $<

{}.c{$(BIN_DIR)}.obj :
	$(CL) /c $(CLFLAGS) $<

{}.cc{$(BIN_DIR)}.obj :
	$(CL) /c $(CLFLAGS) $<

{}.cpp{$(BIN_DIR)}.obj :
	$(CL) /c $(CLFLAGS) $<


#================================================================
# Actions
#================================================================

all : $(EXES)

hash_test.exe : $(OBJS) $(BIN_DIR)\HashTest.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\hash_test.pdb" $(OBJS) HashTest.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
     