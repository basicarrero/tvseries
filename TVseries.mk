##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=TVseries
ConfigurationName      :=Release
WorkspacePath          := "C:\Users\Basi\Desktop\TVseries"
ProjectPath            := "C:\Users\Basi\Desktop\TVseries"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Basi
Date                   :=02/09/2013
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=gcc
SharedObjectLinkerName :=gcc -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="TVseries.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  -lcomctl32 -luxtheme -lgdi32 -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := gcc
CC       := gcc
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/TVseries$(ObjectSuffix) $(IntermediateDirectory)/TVseries.rc$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/TVseries$(ObjectSuffix): TVseries.c $(IntermediateDirectory)/TVseries$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Users/Basi/Desktop/TVseries/TVseries.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TVseries$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TVseries$(DependSuffix): TVseries.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TVseries$(ObjectSuffix) -MF$(IntermediateDirectory)/TVseries$(DependSuffix) -MM "TVseries.c"

$(IntermediateDirectory)/TVseries$(PreprocessSuffix): TVseries.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TVseries$(PreprocessSuffix) "TVseries.c"

$(IntermediateDirectory)/TVseries.rc$(ObjectSuffix): TVseries.rc
	$(RcCompilerName) -i "C:/Users/Basi/Desktop/TVseries/TVseries.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/TVseries.rc$(ObjectSuffix) $(RcIncludePath)

-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/TVseries$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/TVseries$(DependSuffix)
	$(RM) $(IntermediateDirectory)/TVseries$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/TVseries.rc$(ObjectSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) ".build-release/TVseries"


