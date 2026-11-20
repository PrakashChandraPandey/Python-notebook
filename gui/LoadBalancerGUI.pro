QT += core gui widgets
CONFIG += c++17

TARGET = LoadBalancerGUI
TEMPLATE = app

# module files are in the folder above
INCLUDEPATH += ..

SOURCES += main.cpp \
           mainwindow.cpp \
           ../Request_Module.cpp \
           ../Circular_Queue_Module.cpp \
           ../Server_Module.cpp \
           ../Round_Robin_Algorithm.cpp \
           ../Weighted_Round_Robin_Algorithm.cpp \
           ../Least_Connections_Algorithm.cpp \
           ../Algorithm_Selector_Module.cpp \
           ../Auto_Scaler_Module.cpp \
           ../Request_Generation_Module.cpp \
           ../Logger_Module.cpp \
           ../Load_Balancer_Core_and_Algorithms.cpp \
           ../Server_Failure_Module.cpp \
           ../Server_Recovery_Module.cpp \
           ../Server_Display_Module.cpp \
           ../Queue_Display_Module.cpp

HEADERS += mainwindow.h \
           ../data_structures.h \
           ../ProjectHeaders.h
