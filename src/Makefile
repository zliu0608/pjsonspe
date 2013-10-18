VCPATH=C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC
SDKPATH=C:\Program Files\Microsoft SDKs\Windows\v6.0A 
BWINCLUDEPATH=D:\d\tang\trunk\esms\thirdparty\bw451\include
BWLIBPATH=D:\d\tang\trunk\esms\thirdparty\bw451\lib\debug\win32
RAPIDJSON_INCLUDE_PATH=D:\temp\rapidjson\include
HASH_INCLUDE_PATH=.\hash
INCLUDEPATH=.
GOOGLE_TEST_INCLUDEPATH=.\gtest\include
LIBUV_INCLUDE_PATH=D:\temp\libuv\include
LIBUV_INCLUDE_PATH2=D:\temp\libuv\include\uv-private


BIN_DIR=bin

LINKCMD = link
LINKFLAGS_DEBUG = /nologo  /MACHINE:X64 \
  /libpath:"$(VCPATH)\lib\amd64" /libpath:"$(SDKPATH)\Lib\x64" \
  /libpath:"$(BWLIBPATH)" /libpath:"." /libpath:".\bin" \
  /libpath:".\gtest\libs\debug64" \
  /libpath:".\tcmalloc\debug64" \
  /DEBUG \
  /INCREMENTAL:NO \
  /OUT:"$(BIN_DIR)\$@" 
  
LINKFLAGS_RELEASE = /nologo  /MACHINE:X64 \
  /libpath:"$(VCPATH)\lib\amd64" /libpath:"$(SDKPATH)\Lib\x64" \
  /libpath:"$(BWLIBPATH)" /libpath:"." /libpath:".\bin" \
  /libpath:".\gtest\libs\release64" \
  /libpath:".\tcmalloc\release64" \
  /DEBUG \
  /INCREMENTAL:NO \
  /OUT:"$(BIN_DIR)\$@" 
  
    
CLFLAGS_RELEASE = /I"$(BWINCLUDEPATH)" /I"$(INCLUDEPATH)" \
          /I"$(RAPIDJSON_INCLUDE_PATH)" \
          /I"$(HASH_INCLUDE_PATH)" \
          /I"$(GOOGLE_TEST_INCLUDEPATH)" \
          /I"$(LIBUV_INCLUDE_PATH)" \
          /I"$(LIBUV_INCLUDE_PATH2)" \
         /DNDEBUG /D "WIN32" \
         /D_WIN32_WINNT=0x0502 \
         /D_MSC_VER=1500 \
         /D "MEASURE_LATENCY" \
         /nologo /W3 /WX- /MD \
         /Zi /O2 /Oi /Oy- /EHsc /Gd \
         /GS /Fo"$(BIN_DIR)\\"

CLFLAGS_DEBUG = /I"$(BWINCLUDEPATH)" /I"$(INCLUDEPATH)" \
          /I"$(RAPIDJSON_INCLUDE_PATH)" \
          /I"$(HASH_INCLUDE_PATH)" \
          /I"$(GOOGLE_TEST_INCLUDEPATH)" \
          /I"$(LIBUV_INCLUDE_PATH)" \
          /I"$(LIBUV_INCLUDE_PATH2)" \
          /D "_DEBUG" /D "WIN32" \
          /D_WIN32_WINNT=0x0502 \
          /D_MSC_VER=1500 \
          /D "MEASURE_LATENCY" \
          /nologo /W3 /WX- /MDd \
          /ZI /Gm /Od /Oi /Oy- /EHsc /Gd \
          /Fo"$(BIN_DIR)\\"

#          /D "_TRACK_WAIT_TIMES" \

BUILD_TYPE = RELEASE

!if "$(BUILD_TYPE)" == "RELEASE"
!message Making a release build......
GTEST_LIB = gtest.lib
LIBUV_LIB = D:\temp\libuv\x64\Release\lib\uv.lib
LINKFLAGS = $(LINKFLAGS_RELEASE)
CLFLAGS = $(CLFLAGS_RELEASE)
!elseif "$(BUILD_TYPE)" == "DEBUG"
!message Making a debug build......
GTEST_LIB = gtestd.lib
LIBUV_LIB = D:\temp\libuv\x64\Debug\lib\uv.lib
LINKFLAGS = $(LINKFLAGS_DEBUG)
CLFLAGS = $(CLFLAGS_DEBUG)
!else
!message Invalid build type "$(BUILD_TYPE)" specified.
!endif
                  
CL = cl

_VC_MANIFEST_EMBED_EXE= \
if exist $@.manifest mt.exe -manifest $@.manifest -outputresource:$@;1 

EXES= test_engine.exe \
	  test_ringbuffer.exe \
      expr_test.exe \
      transform_test.exe \
      topology_test.exe \
      group_test.exe \
      timewindow_test.exe \
      static_analysis_test.exe \
      bench_agg.exe \
      join_test.exe \
      index_test.exe \
      join_agg_test.exe \
      benchmark_watchlist.exe \
      benchmark_s2winjoin_agg.exe \
      benchmark_win2winjoin_agg.exe \
      adaptorfw_test.exe

OBJS= $(BIN_DIR)\common.obj \
      $(BIN_DIR)\scanner.obj \
      $(BIN_DIR)\ringbuffer.obj \
      $(BIN_DIR)\fcthread.obj \
      $(BIN_DIR)\rbstream.obj \
      $(BIN_DIR)\builtinfuncs.obj \
      $(BIN_DIR)\speparser.obj \
      $(BIN_DIR)\reduce.obj \
      $(BIN_DIR)\tokendef.obj \
      $(BIN_DIR)\GeneralHashFunctions.obj

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

{fcthread/}.cpp.obj :
	$(CL) /c $(CLFLAGS) $<

{hash/}.cpp.obj :
	$(CL) /c $(CLFLAGS) $<

{test/}.cpp.obj :
	$(CL) /c $(CLFLAGS) $<

#================================================================
# Actions
#================================================================

all : $(EXES)

test_engine.exe : $(OBJS) $(BIN_DIR)\test_engine.obj
  $(LINKCMD) $(LINKFLAGS) /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\test_engine.pdb" $(OBJS) test_engine.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcmalloc_minimal.lib

expr_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\expr_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\expr_test.pdb" $(OBJS) jsonspeutils.obj expr_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

transform_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\transform_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\transform_test.pdb" $(OBJS) jsonspeutils.obj transform_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

topology_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\topology_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\topology_test.pdb" $(OBJS) jsonspeutils.obj topology_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

group_test.exe :  $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\group_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\group_test.pdb" $(OBJS) jsonspeutils.obj group_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

timewindow_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\timewindow_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\timewindow_test.pdb" $(OBJS) jsonspeutils.obj timewindow_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 
     
static_analysis_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\static_analysis_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\static_analysis_test.pdb" $(OBJS) jsonspeutils.obj static_analysis_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

bench_agg.exe :  $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_aggregation.obj
  $(LINKCMD) $(LINKFLAGS)  /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\bench_agg.pdb" $(OBJS) jsonspeutils.obj benchmark_aggregation.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcmalloc_minimal.lib      

benchmark_watchlist.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_watchlist.obj
  $(LINKCMD) $(LINKFLAGS)  /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\benchmark_watchlist.pdb" $(OBJS) jsonspeutils.obj benchmark_watchlist.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcmalloc_minimal.lib  

benchmark_s2winjoin_agg.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_s2winjoin_agg.obj
  $(LINKCMD) $(LINKFLAGS) /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\benchmark_s2winjoin_agg.pdb" $(OBJS) jsonspeutils.obj benchmark_s2winjoin_agg.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  libtcmalloc_minimal.lib  

benchmark_win2winjoin_agg.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_win2winjoin_agg.obj
  $(LINKCMD) $(LINKFLAGS) /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\benchmark_win2winjoin_agg.pdb" $(OBJS) jsonspeutils.obj benchmark_win2winjoin_agg.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  libtcmalloc_minimal.lib  

benchmark_tukp1a.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_tukp1a.obj $(BIN_DIR)\arcanum_client.obj
  $(LINKCMD) $(LINKFLAGS)  /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\benchmark_watchlist.pdb" $(OBJS) jsonspeutils.obj benchmark_tukp1a.obj arcanum_client.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcmalloc_minimal.lib \
     ws2_32.lib psapi.lib iphlpapi.lib $(LIBUV_LIB)
  CD $(BIN_DIR)
  $(_VC_MANIFEST_EMBED_EXE)
  CD ..

tukp1a_socket.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\benchmark_tukp1a.obj $(BIN_DIR)\socket_client_win.obj
  $(LINKCMD) $(LINKFLAGS)  /INCLUDE:"__tcmalloc" /PDB:"$(BIN_DIR)\tukp1a_socket.pdb" $(OBJS) jsonspeutils.obj benchmark_tukp1a.obj socket_client_win.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcmalloc_minimal.lib \
     ws2_32.lib psapi.lib iphlpapi.lib
  CD $(BIN_DIR)
  $(_VC_MANIFEST_EMBED_EXE)
  CD ..

join_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\join_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\join_test.pdb" $(OBJS) jsonspeutils.obj join_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 
     
index_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\index_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\index_test.pdb" $(OBJS) jsonspeutils.obj index_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

join_agg_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\join_agg_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\join_agg_test.pdb" $(OBJS) jsonspeutils.obj join_agg_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 
     
temp_test.exe : $(OBJS) $(BIN_DIR)\jsonspeutils.obj $(BIN_DIR)\temp_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\temp_test.pdb" $(OBJS) jsonspeutils.obj temp_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
     $(GTEST_LIB) 

test_ringbuffer.exe : $(BIN_DIR)\ringbuffer.obj $(BIN_DIR)\fcthread.obj $(BIN_DIR)\test_ringbuffer.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\test_ringbuffer.pdb" ringbuffer.obj fcthread.obj test_ringbuffer.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

adaptorfw_test.exe : $(OBJS) $(BIN_DIR)\adaptorfw_test.obj
  $(LINKCMD) $(LINKFLAGS) /PDB:"$(BIN_DIR)\adaptorfw_test.pdb" $(OBJS) adaptorfw_test.obj \
     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
     shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

clean :
	-del *.obj *.lib  *.exp *.exe *.dll *.manifest *.ilk *.pdb /F /Q > NUL: 2>&1
	-cd bin
	-del *.obj *.lib  *.exp *.exe *.dll *.manifest *.ilk *.pdb /F /Q > NUL: 2>&1
	