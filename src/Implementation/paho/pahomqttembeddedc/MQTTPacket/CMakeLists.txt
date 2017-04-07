
set(SOURCE_FILES
        src/MQTTConnect.h
        src/MQTTConnectClient.c
        src/MQTTConnectServer.c
        src/MQTTDeserializePublish.c
        src/MQTTFormat.c
        src/MQTTFormat.h
        src/MQTTPacket.c
        src/MQTTPacket.h
        src/MQTTPublish.h
        src/MQTTSerializePublish.c
        src/MQTTSubscribe.h
        src/MQTTSubscribeClient.c
        src/MQTTSubscribeServer.c
        src/MQTTUnsubscribe.h
        src/MQTTUnsubscribeClient.c
        src/MQTTUnsubscribeServer.c
        src/StackTrace.h
        )

add_library(MQTTPacket ${SOURCE_FILES})