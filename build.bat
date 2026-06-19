@echo off
setlocal

g++ -std=c++17 -Iinclude src\main.cpp -lgdi32 -o GameTree.exe
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build complete: GameTree.exe
