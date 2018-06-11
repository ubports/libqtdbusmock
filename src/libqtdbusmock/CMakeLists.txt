
set(
	QTDBUSMOCK_SRC
	DBusMock.cpp
	Method.cpp
	MethodCall.cpp
	NamedMethodCall.cpp
)

set(
	QTDBUSMOCK_PUBLIC_HEADERS
	"${CMAKE_CURRENT_BINARY_DIR}/config.h"
	DBusMock.h
	DeclareMetatypes.h
	ExportInterfaces.h
	Method.h
	MethodCall.h
	MockInterfaceClasses.h
	NamedMethodCall.h
	"${CMAKE_CURRENT_BINARY_DIR}/MockInterface.h"
	"${CMAKE_CURRENT_BINARY_DIR}/PropertiesInterface.h"
	"${CMAKE_CURRENT_BINARY_DIR}/NetworkManagerMockInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/NotificationDaemonMockInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OfonoMockInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OfonoModemInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OfonoConnectionManagerInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OfonoSimManagerInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OfonoNetworkRegistrationInterface.h"
        "${CMAKE_CURRENT_BINARY_DIR}/URfkillInterface.h"
	"${CMAKE_CURRENT_BINARY_DIR}/URfkillDeviceInterface.h"
	"${CMAKE_CURRENT_BINARY_DIR}/URfkillKillswitchInterface.h"
)

set_source_files_properties(
	"${DATA_DIR}/org.freedesktop.DBus.Mock.xml"
	PROPERTIES
	NO_NAMESPACE YES
	INCLUDE "libqtdbusmock/MockInterfaceClasses.h"
)

set_source_files_properties(
	"${DATA_DIR}/org.freedesktop.DBus.Properties.xml"
	PROPERTIES
	NO_NAMESPACE YES
	INCLUDE "libqtdbusmock/ExportInterfaces.h"
)

set_source_files_properties(
	"${DATA_DIR}/nm-manager-mock.xml"
	PROPERTIES
	NO_NAMESPACE YES
	CLASSNAME NetworkManagerMockInterface
	INCLUDE "libqtdbusmock/ExportInterfaces.h"
)

set_source_files_properties(
        "${DATA_DIR}/notification-daemon-mock.xml"
        PROPERTIES
        NO_NAMESPACE YES
        CLASSNAME NotificationDaemonMockInterface
)

set_source_files_properties(
	"${DATA_DIR}/ofono-mock.xml"
	PROPERTIES
	NO_NAMESPACE YES
	CLASSNAME OfonoMockInterface
)

set_source_files_properties(
        "${DATA_DIR}/ofono-modem-mock.xml"
        PROPERTIES
        NO_NAMESPACE YES
        CLASSNAME OfonoModemInterface
)

set_source_files_properties(
        "${DATA_DIR}/ofono-simmanager.xml"
        PROPERTIES
        NO_NAMESPACE YES
        CLASSNAME OfonoSimManagerInterface
)

set_source_files_properties(
        "${DATA_DIR}/ofono-connectionmanager.xml"
        PROPERTIES
        NO_NAMESPACE YES
        CLASSNAME OfonoConnectionManagerInterface
)

set_source_files_properties(
        "${DATA_DIR}/ofono-networkregistration.xml"
        PROPERTIES
        NO_NAMESPACE YES
        CLASSNAME OfonoNetworkRegistrationInterface
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/org.freedesktop.DBus.Mock.xml"
	MockInterface
)	

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/org.freedesktop.DBus.Properties.xml"
	PropertiesInterface
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/nm-manager-mock.xml"
	NetworkManagerMockInterface
)

qt5_add_dbus_interface(
        QTDBUSMOCK_SRC
        "${DATA_DIR}/notification-daemon-mock.xml"
        NotificationDaemonMockInterface
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/ofono-mock.xml"
	OfonoMockInterface
)

qt5_add_dbus_interface(
        QTDBUSMOCK_SRC
        "${DATA_DIR}/ofono-modem-mock.xml"
        OfonoModemInterface
)

qt5_add_dbus_interface(
        QTDBUSMOCK_SRC
        "${DATA_DIR}/ofono-connectionmanager.xml"
        OfonoConnectionManagerInterface
)

qt5_add_dbus_interface(
        QTDBUSMOCK_SRC
        "${DATA_DIR}/ofono-simmanager.xml"
        OfonoSimManagerInterface
)

qt5_add_dbus_interface(
        QTDBUSMOCK_SRC
        "${DATA_DIR}/ofono-networkregistration.xml"
        OfonoNetworkRegistrationInterface
)

set_source_files_properties(
	"${DATA_DIR}/org.freedesktop.URfkill.xml"
	PROPERTIES
	NO_NAMESPACE YES
)

set_source_files_properties(
	"${DATA_DIR}/org.freedesktop.URfkill.Device.xml"
	PROPERTIES
	NO_NAMESPACE YES
)

set_source_files_properties(
	"${DATA_DIR}/org.freedesktop.URfkill.Killswitch.xml"
	PROPERTIES
	NO_NAMESPACE YES
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/org.freedesktop.URfkill.xml"
	URfkillInterface
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/org.freedesktop.URfkill.Device.xml"
	URfkillDeviceInterface
)

qt5_add_dbus_interface(
	QTDBUSMOCK_SRC
	"${DATA_DIR}/org.freedesktop.URfkill.Killswitch.xml"
	URfkillKillswitchInterface
)

add_library(
	qtdbusmock
	SHARED
	${QTDBUSMOCK_SRC}
)

target_link_libraries(
    qtdbusmock
    Qt5::Core
    Qt5::DBus
    Qt5::Test
)

target_link_libraries(
	qtdbusmock
	${QTDBUSTEST_LIBRARIES}
)

set_target_properties(
	qtdbusmock
	PROPERTIES
	VERSION ${API_VERSION}.0.0
	SOVERSION ${ABI_VERSION}
	PUBLIC_HEADER "${QTDBUSMOCK_PUBLIC_HEADERS}"
)

install(
	TARGETS qtdbusmock
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libqtdbusmock-${API_VERSION}/libqtdbusmock"
)

# Package config

set(LIBQTDBUSMOCK_PC "${CMAKE_CURRENT_BINARY_DIR}/libqtdbusmock-${API_VERSION}.pc")
configure_file("libqtdbusmock.pc.in" "${LIBQTDBUSMOCK_PC}" @ONLY)

install(
	FILES "${LIBQTDBUSMOCK_PC}"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)
