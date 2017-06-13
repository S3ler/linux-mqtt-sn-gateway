
set(SOURCE_FILES
        TcpSocket.h
        TcpSocket.cpp
        ConnectionAcceptor.h
        ConnectionAcceptor.cpp
        TcpConnection.h
        TcpConnection.cpp
        TcpMessage.h
        TcpMessage.cpp
        Queue.h
        )

add_library(tcpSocket ${SOURCE_FILES})

set(TCP_SOCKET_MAIN tcpmain.cpp)

add_executable(tcpSocketMain ${SOURCE_FILES} ${TCP_SOCKET_MAIN})

