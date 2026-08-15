#include "SettingsManager.h"
#include "settings_keys.h"
#include <QFileInfo>
#include <QSettings>

constexpr int MAX_RECENT_FILES = 10;

using namespace std;


template <typename T>
void setSetting(const QString & filePath, const QString groupName, T value)
{
	QSettings settings;

	settings.beginGroup(groupName);
	settings.setValue(filePath, value);
	settings.endGroup();
}

template<typename T>
optional<T> getSetting(const QString& filePath, const QString& groupName)
{
	QSettings settings;

	settings.beginGroup(groupName);
	const bool hasValue = settings.contains(filePath);
	const QVariant value = settings.value(filePath, groupName);
	settings.endGroup();

	if (!hasValue)
	{
		return nullopt;
	}

	if constexpr (std::is_same_v<T, QString>)
	{
		return value.toString();
	}
	else if constexpr (std::is_same_v<T, int>)
	{
		return value.toInt();
	}
	else if constexpr (std::is_same_v<T, double>)
	{
		return value.toDouble();
	}

	return nullopt;
}

QString SettingsManager::getLastDir()
{
	QSettings settings;

	settings.beginGroup(Settings::GeneralGroup);
	QString lastDir = settings.value(Settings::LastDir,QString()).toString();
	settings.endGroup();

	return lastDir;
}

void SettingsManager::setLastDir(
	const QString & dir
)
{
	QSettings settings;
	settings.beginGroup(Settings::GeneralGroup);
	settings.setValue(Settings::LastDir,dir);
	settings.endGroup();
}

QStringList SettingsManager::getRecentFiles()
{
	QSettings settings;
	settings.beginGroup(Settings::RecentFilesGroup);
	QStringList files = settings.value(Settings::FilesList).toStringList();

	QStringList cleanFiles;
	for (const QString & filePath : files)
	{
		bool exists = QFileInfo::exists(filePath);
		if (exists && !cleanFiles.contains(filePath))
		{
			cleanFiles.append(filePath);
		}
	}
	if (cleanFiles.size() != files.size())
	{
		settings.setValue(Settings::FilesList,cleanFiles);
	}
	settings.endGroup();
	return cleanFiles;
}

void SettingsManager::addRecentFile(
	const QString & fileName
)
{
	QSettings settings;
	settings.beginGroup(Settings::RecentFilesGroup);
	QStringList files = settings.value(Settings::FilesList).toStringList();

	files.removeAll(fileName);
	files.prepend(fileName);

	while (files.size() > MAX_RECENT_FILES)
	{
		files.removeLast();
	}

	settings.setValue(Settings::FilesList,files);
	settings.endGroup();
}

void SettingsManager::setRecentFiles(
	const QStringList & files
)
{
	QSettings settings;
	settings.beginGroup(Settings::RecentFilesGroup);
	settings.setValue(Settings::FilesList,files);
	settings.endGroup();
}

void SettingsManager::setLastPage(const QString& filePath, int pageNumber)
{
	setSetting(filePath, Settings::LastPageGroup, pageNumber);
}

optional<int> SettingsManager::getLastPage(const QString & filePath)
{
	return getSetting<int>(filePath, Settings::LastPageGroup);
}

void SettingsManager::setZoom(const QString& filePath, double zoom)
{
	setSetting(filePath, Settings::ZoomGroup, zoom);
}

optional<double> SettingsManager::getZoom(const QString & filePath)
{
	return getSetting<double>(filePath, Settings::ZoomGroup);
}
