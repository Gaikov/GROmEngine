cmake_minimum_required(VERSION 3.10)
project(Core)

set(sources
        Args.cpp
        Config.cpp
        Core.cpp
        FileReader.cpp
        FileWriter.cpp
        MemReader.cpp
        Memory.cpp
        Name.cpp
        Net.cpp
        Package.cpp
        ParseFile.cpp
        Parser.cpp
        ParserUtils.cpp
        ScriptSaver.cpp
        SmartPtr.cpp
        Var.cpp
        headers.cpp
        sys.cpp
        BitmapData.cpp
        debug/LogFile.cpp
        debug/LogStdOut.cpp
        Blob.cpp
        Blob.h
        Crypt.cpp
        Crypt.h
        PackArch.cpp
        Profiler.cpp
        DataWriter.cpp
        undo/UndoService.cpp
        undo/UndoService.h
        undo/UndoRedoOperation.h
        undo/UndoVarChange.h
        undo/UndoBatch.cpp
        undo/UndoBatch.h
        undo/UndoVectorInsert.h
)

include_directories(
        ./
)

add_library(Core STATIC ${sources})