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
#include <libqtdbustest/QProcessDBusService.h>

#include <nm-dbus-interface.h>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

using namespace QtDBusTest;

namespace QtDBusMock {

class DBusMockPrivate {
public:
	DBusMockPrivate(DBusTestRunner &testRunner) :
			m_testRunner(testRunner) {
	}

	QtDBusTest::DBusTestRunner &m_testRunner;

	QScopedPointer<NetworkManagerMockInterface> m_networkManagerMock;

	QScopedPointer<NotificationDaemonMockInterface> m_notificationDaemonMock;

	QScopedPointer<OfonoMockInterface> m_ofonoInterface;

	QMap<QString, QSharedPointer<OfonoModemInterface>> m_ofonoModemInterface;

	QMap<QString, QSharedPointer<OfonoSimManagerInterface>> m_ofonoSimManagerInterface;

	QMap<QString, QSharedPointer<OfonoConnectionManagerInterface>> m_ofonoConnectionManagerInterface;

	QMap<QString, QSharedPointer<OfonoNetworkRegistrationInterface>> m_ofonoNetworkRegistrationInterface;

	QScopedPointer<OrgFreedesktopURfkillInterface> m_urfkillInterface;

	QMap<QString, QSharedPointer<OrgFreedesktopURfkillKillswitchInterface> > m_urfkillKillSwitchInterfaces;

	QMap<QString, QSharedPointer<OrgFreedesktopDBusMockInterface> > m_mockInterfaces;
};

DBusMock::DBusMock(DBusTestRunner &testRunner) :
		d(new DBusMockPrivate(testRunner)) {
}

DBusMock::~DBusMock() {
}

void DBusMock::registerMetaTypes() {
	Method::registerMetaType();
	MethodCall::registerMetaType();
	NamedMethodCall::registerMetaType();

    qRegisterMetaType<VariantDictMap>("QtDBusMock::VariantDictMap");
    qDBusRegisterMetaType<VariantDictMap>();
}

void DBusMock::registerTemplate(const QString &service,
                                const QString &templateName,
                                QDBusConnection::BusType busType) {
	registerTemplate(service, templateName, QVariantMap(), busType);
}


void DBusMock::registerTemplate(const QString &service,
                              const QString &templateName,
                              const QVariantMap& parameters,
                              QDBusConnection::BusType busType) {
	QStringList arguments;
	arguments << "-m" << "dbusmock";

	bool found = false;
	if (!templateName.contains(QDir::separator())) {
		auto dirs = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
		for (const auto & dir : dirs) {
			auto path = QDir(QDir(QDir(dir).filePath("libqtdbusmock")).filePath("templates")).filePath(templateName + ".py");
			if (QFileInfo::exists(path)) {
				arguments << "--template" << path;
				found = true;
				break;
			}
		}
	}
	if (!found) {
		arguments << "--template" << templateName;
	}

	if (!parameters.isEmpty())
	{
		auto doc = QJsonDocument::fromVariant(parameters);
		arguments << "--parameters" << QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
	}

	d->m_testRunner.registerService(
			DBusServicePtr(
					new QProcessDBusService(service, busType, "python3",
							arguments)));
}

void DBusMock::registerBluez5(const QVariantMap& parameters) {
	registerTemplate("org.bluez", "bluez5", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerBluez4(const QVariantMap& parameters) {
	registerTemplate("org.bluez", "bluez4", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerGnomeScreensaver(const QVariantMap& parameters) {
	registerTemplate("org.gnome.ScreenSaver", "gnome_screensaver", parameters,
			QDBusConnection::SessionBus);
}

void DBusMock::registerLogind(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.login1", "logind", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerNetworkManager() {
	registerNetworkManager(QVariantMap());
}

void DBusMock::registerNetworkManager(const QVariantMap& parameters) {
	registerTemplate(NM_DBUS_SERVICE, "networkmanager", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerNotificationDaemon() {
	registerNotificationDaemon(QVariantMap());
}

void DBusMock::registerNotificationDaemon(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.Notifications", "notification_daemon",
			parameters, QDBusConnection::SessionBus);
}

void DBusMock::registerOfono() {
	registerOfono(QVariantMap());
}

void DBusMock::registerOfono(const QVariantMap& parameters) {
	registerTemplate("org.ofono", "ofono", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerPolicyKit(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.PolicyKit1", "polkitd", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerTimeDate(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.timedate1", "timedated", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerHostname1(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.hostname1", "org.freedesktop.hostname1", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerLogin1(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.login1", "org.freedesktop.login1", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerURfkill() {
	registerURfkill(QVariantMap());
}

void DBusMock::registerUpower(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.UPower", "upower", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerURfkill(const QVariantMap& parameters) {
	registerTemplate("org.freedesktop.URfkill", "urfkill", parameters,
			QDBusConnection::SystemBus);
}

void DBusMock::registerCustomMock(const QString &name, const QString &path,
		const QString &interface, QDBusConnection::BusType busType) {
	QStringList args;
	args << "-m" << "dbusmock";
	if (busType == QDBusConnection::SystemBus) {
		args << "-s";
	}
	args << name << path << interface;
	d->m_testRunner.registerService(
			DBusServicePtr(
					new QProcessDBusService(name, busType, "python3", args)));
}

NetworkManagerMockInterface & DBusMock::networkManagerInterface() {
	if (d->m_networkManagerMock.isNull()) {
		d->m_networkManagerMock.reset(
				new NetworkManagerMockInterface(NM_DBUS_INTERFACE, NM_DBUS_PATH,
						d->m_testRunner.systemConnection()));
	}
	return *d->m_networkManagerMock;
}

NotificationDaemonMockInterface & DBusMock::notificationDaemonInterface() {
	if (d->m_notificationDaemonMock.isNull()) {
		d->m_notificationDaemonMock.reset(
				new NotificationDaemonMockInterface("org.freedesktop.Notifications",
						"/org/freedesktop/Notifications",
						d->m_testRunner.sessionConnection()));
	}
	return *d->m_notificationDaemonMock;
}

OfonoMockInterface & DBusMock::ofonoInterface() {
	if (d->m_ofonoInterface.isNull()) {
		d->m_ofonoInterface.reset(
				new OfonoMockInterface("org.ofono", "/",
						d->m_testRunner.systemConnection()));
	}
	return *d->m_ofonoInterface;
}

OfonoModemInterface & DBusMock::ofonoModemInterface(const QString& path) {
	QSharedPointer<OfonoModemInterface> interface =
			d->m_ofonoModemInterface[path];
	if (!interface) {
		interface.reset(
				new OfonoModemInterface("org.ofono", path,
						d->m_testRunner.systemConnection()));
		d->m_ofonoModemInterface[path] = interface;
	}
	return *interface;
}

OfonoSimManagerInterface & DBusMock::ofonoSimManagerInterface(const QString& path) {
	QSharedPointer<OfonoSimManagerInterface> interface =
			d->m_ofonoSimManagerInterface[path];
	if (!interface) {
		interface.reset(
				new OfonoSimManagerInterface("org.ofono", path,
						d->m_testRunner.systemConnection()));
		d->m_ofonoSimManagerInterface[path] = interface;
	}
	return *interface;
}

OfonoConnectionManagerInterface & DBusMock::ofonoConnectionManagerInterface(const QString& path) {
	QSharedPointer<OfonoConnectionManagerInterface> interface =
			d->m_ofonoConnectionManagerInterface[path];
	if (!interface) {
		interface.reset(
				new OfonoConnectionManagerInterface("org.ofono", path,
						d->m_testRunner.systemConnection()));
		d->m_ofonoConnectionManagerInterface[path] = interface;
	}
	return *interface;
}

OfonoNetworkRegistrationInterface & DBusMock::ofonoNetworkRegistrationInterface(const QString& path) {
	QSharedPointer<OfonoNetworkRegistrationInterface> interface =
			d->m_ofonoNetworkRegistrationInterface[path];
	if (!interface) {
		interface.reset(
				new OfonoNetworkRegistrationInterface("org.ofono", path,
						d->m_testRunner.systemConnection()));
		d->m_ofonoNetworkRegistrationInterface[path] = interface;
	}
	return *interface;
}

OrgFreedesktopURfkillInterface & DBusMock::urfkillInterface() {
	if (d->m_urfkillInterface.isNull()) {
		d->m_urfkillInterface.reset(
				new OrgFreedesktopURfkillInterface(
						"org.freedesktop.URfkill", "/org/freedesktop/URfkill",
						d->m_testRunner.systemConnection()));
	}
	return *d->m_urfkillInterface;
}

OrgFreedesktopURfkillKillswitchInterface & DBusMock::urfkillKillswitchInterface(const QString& device)
{
	QSharedPointer<OrgFreedesktopURfkillKillswitchInterface> interface = d->m_urfkillKillSwitchInterfaces[device];
	if (!interface) {
		interface.reset(
				new OrgFreedesktopURfkillKillswitchInterface(
						"org.freedesktop.URfkill",
						QString("/org/freedesktop/URfkill/%1").arg(device),
						d->m_testRunner.systemConnection()));
		d->m_urfkillKillSwitchInterfaces[device] = interface;
	}
	return *interface;
}

OrgFreedesktopDBusMockInterface & DBusMock::mockInterface(const QString &name,
		const QString &path, const QString &interface,
		QDBusConnection::BusType busType) {
	Q_UNUSED(interface);
	QString id(name);
	id.append(':');
	id.append(path);
	auto it(d->m_mockInterfaces.find(id));
	if (it == d->m_mockInterfaces.end()) {
		switch (busType) {
		case QDBusConnection::SystemBus:
			it = d->m_mockInterfaces.insert(id,
					QSharedPointer<OrgFreedesktopDBusMockInterface>(
							new OrgFreedesktopDBusMockInterface(name, path,
									d->m_testRunner.systemConnection())));
			break;
		case QDBusConnection::SessionBus:
			it = d->m_mockInterfaces.insert(id,
					QSharedPointer<OrgFreedesktopDBusMockInterface>(
							new OrgFreedesktopDBusMockInterface(name, path,
									d->m_testRunner.sessionConnection())));
			break;
		case QDBusConnection::ActivationBus:
			qWarning() << "Unknown bus type";
			break;
		}
	}
	return **it;
}

}
