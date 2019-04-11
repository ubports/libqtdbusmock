/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <libqtdbusmock/DBusMock.h>
#include <QCoreApplication>
#include <nm-dbus-interface.h>
#include <QList>
#include <QDBusInterface>
#include <QDBusObjectPath>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace QtDBusTest;
using namespace QtDBusMock;

namespace {

class TestDBusMock: public Test {
protected:
	TestDBusMock() :
			dbusMock(dbusTestRunner) {
	}

	virtual ~TestDBusMock() {
	}

	bool processListContains(const char *str) {
		QProcess pgrep;
		pgrep.start("ps",
				QStringList() << "--no-headers" << "--ppid"
						<< QString::number(QCoreApplication::applicationPid())
						<< "-o" << "args");
		pgrep.waitForFinished();
		pgrep.waitForReadyRead();

		return pgrep.readAll().contains(str);
	}

	DBusTestRunner dbusTestRunner;

	DBusMock dbusMock;
};

TEST_F(TestDBusMock, StartsDBusMockSession) {
	dbusMock.registerCustomMock("test.name", "/test/object", "test.Interface",
			QDBusConnection::SessionBus);
	dbusTestRunner.startServices();

	EXPECT_TRUE(
			processListContains(
					"python3 -m dbusmock test.name /test/object test.Interface"));
}

TEST_F(TestDBusMock, StartsDBusMockSystem) {
	dbusMock.registerCustomMock("test.name2", "/test/object2",
			"test.Interface2", QDBusConnection::SystemBus);
	dbusTestRunner.startServices();

	EXPECT_TRUE(
			processListContains(
					"python3 -m dbusmock -s test.name2 /test/object2 test.Interface2"));
}

TEST_F(TestDBusMock, StartsDBusMockWithNM) {
	dbusMock.registerNetworkManager();
	dbusTestRunner.startServices();

	EXPECT_TRUE(
			processListContains(
					"python3 -m dbusmock --template networkmanager"));

	NetworkManagerMockInterface &networkManager(
			dbusMock.networkManagerInterface());
	networkManager.AddWiFiDevice("device", "eth1", NM_STATE_DISCONNECTED).waitForFinished();

	QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH,
	                     NM_DBUS_INTERFACE,
	                     dbusTestRunner.systemConnection());

	QDBusReply<QList<QDBusObjectPath> > devices = iface.call("GetDevices");

	ASSERT_EQ(true, devices.isValid());
	ASSERT_EQ(1, devices.value().size());
}

TEST_F(TestDBusMock, StartsDBusMockTemplateParameters) {
	dbusMock.registerUpower({{"DaemonVersion", "0.99.0"}, {"OnBattery", true}});
	dbusTestRunner.startServices();

	EXPECT_TRUE(
			processListContains(
					"python3 -m dbusmock --template upower --parameters {\"DaemonVersion\":\"0.99.0\",\"OnBattery\":true}"));

	QDBusInterface iface("org.freedesktop.UPower", "/org/freedesktop/UPower",
			"org.freedesktop.UPower", dbusTestRunner.systemConnection());

	EXPECT_EQ("0.99.0", iface.property("DaemonVersion").toString());
	EXPECT_TRUE(iface.property("OnBattery").toBool());
}

TEST_F(TestDBusMock, StartsDBusMockWithTemplate) {
	dbusMock.registerTemplate(NM_DBUS_SERVICE,
                                  "networkmanager",
                                  QDBusConnection::SystemBus);
	dbusTestRunner.startServices();

	EXPECT_TRUE(
			processListContains(
					"python3 -m dbusmock --template networkmanager"));

	NetworkManagerMockInterface &networkManager(
			dbusMock.networkManagerInterface());
	networkManager.AddWiFiDevice("device", "eth1", NM_STATE_DISCONNECTED).waitForFinished();

	QDBusInterface iface (NM_DBUS_SERVICE, NM_DBUS_PATH,
	                      NM_DBUS_INTERFACE,
	                      dbusTestRunner.systemConnection());

	QDBusReply<QList<QDBusObjectPath> > devices = iface.call("GetDevices");

	ASSERT_EQ(true, devices.isValid());
	ASSERT_EQ(1, devices.value().size());
}

TEST_F(TestDBusMock, GetMethodCalls) {
	dbusMock.registerCustomMock("test.name", "/test/path", "test.interface",
			QDBusConnection::SessionBus);
	dbusTestRunner.startServices();

	OrgFreedesktopDBusMockInterface &mockInterface(
			dbusMock.mockInterface("test.name", "/test/path", "test.interface",
					QDBusConnection::SessionBus));
	mockInterface.AddMethod("test.interface", "MethodName", "s", "s",
			"ret = 'output'").waitForFinished();

	QDBusMessage message(
			QDBusMessage::createMethodCall("test.name", "/test/path",
					"test.interface", "MethodName"));
	message.setArguments(QVariantList() << QVariant("input"));
	QDBusMessage reply(dbusTestRunner.sessionConnection().call(message));
	ASSERT_EQ(1, reply.arguments().size());
	EXPECT_EQ("output", reply.arguments().first().toString().toStdString());

	QList<MethodCall> methodCalls(mockInterface.GetMethodCalls("MethodName"));
	ASSERT_EQ(1, methodCalls.size());
	MethodCall call;
	call = methodCalls.first();
	ASSERT_EQ(1, call.args().size());
	EXPECT_EQ("input", call.args().first().toString().toStdString());
}

TEST_F(TestDBusMock, GetAllMethodCalls) {
	dbusMock.registerCustomMock("test.name", "/test/path", "test.interface",
			QDBusConnection::SessionBus);
	dbusTestRunner.startServices();

	OrgFreedesktopDBusMockInterface &mockInterface(
			dbusMock.mockInterface("test.name", "/test/path", "test.interface",
					QDBusConnection::SessionBus));
	mockInterface.AddMethod("test.interface", "MethodName", "s", "s",
			"ret = 'output'").waitForFinished();

	QDBusMessage message(
			QDBusMessage::createMethodCall("test.name", "/test/path",
					"test.interface", "MethodName"));
	message.setArguments(QVariantList() << QVariant("input"));
	QDBusMessage reply(dbusTestRunner.sessionConnection().call(message));
	ASSERT_EQ(1, reply.arguments().size());
	EXPECT_EQ("output", reply.arguments().first().toString().toStdString());

	QList<NamedMethodCall> methodCalls(mockInterface.GetCalls());
	ASSERT_EQ(1, methodCalls.size());
	NamedMethodCall call;
	call = methodCalls.first();
	EXPECT_EQ("MethodName", call.methodName().toStdString());
	ASSERT_EQ(1, call.args().size());
	EXPECT_EQ("input", call.args().first().toString().toStdString());
}

TEST_F(TestDBusMock, AddMethods) {
	dbusMock.registerCustomMock("test.name", "/test/path", "test.interface",
			QDBusConnection::SessionBus);
	dbusTestRunner.startServices();

	OrgFreedesktopDBusMockInterface &mockInterface(
			dbusMock.mockInterface("test.name", "/test/path", "test.interface",
					QDBusConnection::SessionBus));

	Method method1;
	method1.setName("MethodName1");
	method1.setInSig("s");
	method1.setOutSig("s");
	method1.setCode("ret = 'output1'");

	Method methodTemp;
	methodTemp.setName("MethodName2");
	methodTemp.setInSig("u");
	methodTemp.setOutSig("s");
	methodTemp.setCode("ret = 'output2'");

	Method method2;
	method2 = methodTemp;

	mockInterface.AddMethods("test.interface",
			QList<Method>() << method1 << method2).waitForFinished();

	{
		QDBusMessage message(
				QDBusMessage::createMethodCall("test.name", "/test/path",
						"test.interface", "MethodName1"));
		message.setArguments(QVariantList() << QVariant("input"));
		QDBusMessage reply(dbusTestRunner.sessionConnection().call(message));
		ASSERT_EQ(1, reply.arguments().size());
		EXPECT_EQ("output1",
				reply.arguments().first().toString().toStdString());
	}

	{
		QDBusMessage message(
				QDBusMessage::createMethodCall("test.name", "/test/path",
						"test.interface", "MethodName2"));
		message.setArguments(QVariantList() << QVariant(123));
		QDBusMessage reply(dbusTestRunner.sessionConnection().call(message));
		ASSERT_EQ(1, reply.arguments().size());
		EXPECT_EQ("output2",
				reply.arguments().first().toString().toStdString());
	}

	QList<NamedMethodCall> methodCalls(mockInterface.GetCalls());
	ASSERT_EQ(2, methodCalls.size());
	{
		NamedMethodCall call;
		call = methodCalls.first();
		EXPECT_EQ("MethodName1", call.methodName().toStdString());
		ASSERT_EQ(1, call.args().size());
		EXPECT_EQ("input", call.args().first().toString().toStdString());
	}
	{
		NamedMethodCall call;
		call = methodCalls[1];
		EXPECT_EQ("MethodName2", call.methodName().toStdString());
		ASSERT_EQ(1, call.args().size());
		EXPECT_EQ(123, call.args().first().toUInt());
	}
}

} // namespace
