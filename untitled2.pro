QT += widgets gui core

SOURCES += \
    main.cpp \
    pollendetect.cpp \
    mythread.cpp

HEADERS += \
    pollendetect.h \
    mythread.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lin/opencv3.4.1/build/x64/vc14/lib/ -lopencv_world341
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lin/opencv3.4.1/build/x64/vc14/lib/ -lopencv_world341d
else:unix: LIBS += -L$$PWD/../../lin/opencv3.4.1/build/x64/vc14/lib/ -lopencv_world341

CONFIG(debug, debug|release):{ LIBS += -L$$PWD/../../lin/opencv3.4.1/build/x64/vc14/bin/ -lopencv_world341d -lopencv_world341d}#这里偷懒。。。

INCLUDEPATH += $$PWD/../../lin/opencv3.4.1/build/include/opencv
DEPENDPATH += $$PWD/../../lin/opencv3.4.1/build/include/opencv
INCLUDEPATH += $$PWD/../../lin/opencv3.4.1/build/include/opencv2
DEPENDPATH += $$PWD/../../lin/opencv3.4.1/build/include/opencv2
INCLUDEPATH += $$PWD/../../lin/opencv3.4.1/build/include
DEPENDPATH += $$PWD/../../lin/opencv3.4.1/build/include
INCLUDEPATH += $$PWD/../../lin/opencv3.4.1/build/x64/vc14
DEPENDPATH += $$PWD/../../lin/opencv3.4.1/build/x64/vc14

FORMS += \
    pollendetect.ui
