cmake_minimum_required(VERSION 3.10.0)
project(MyDictionaryApp_VSCode VERSION 0.1.0 LANGUAGES C CXX)

add_executable(MyDictionaryApp_VSCode main.cpp AddWordDialog.cpp EditWordDialog.cpp TrainDialog.cpp Dictionary.cpp)

target_link_libraries(MyDictionaryApp_VSCode user32 gdi32)

