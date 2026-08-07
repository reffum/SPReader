#include "SettingsManager.h"
#include "settings_keys.h"
#include <QFileInfo>
#include <QSettings>

constexpr int MAX_RECENT_FILES = 10;

QString SettingsManager::getLastDir()
{
	QSettings settings;
	settings.beginGroup(
		Settings::GeneralGroup
	);
	QString lastDir = settings.value(
		Settings::LastDir,
		QString()
	).toString();
	settings.endGroup();
	return lastDir;
}

void SettingsManager::setLastDir(
	const QString & dir
)
{
	QSettings settings;
	settings.beginGroup(
		Settings::GeneralGroup
	);
	settings.setValue(
		Settings::LastDir,
		dir
	);
	settings.endGroup();
}

QStringList SettingsManager::getRecentFiles()
{
	QSettings settings;
	settings.beginGroup(
		Settings::RecentFilesGroup
	);
	QStringList files = settings.value(
		Settings::FilesList
	).toStringList();

	QStringList cleanFiles;
	for (const QString & filePath : files)
	{
		bool exists =
			QFileInfo::exists(filePath);
		if (exists && !cleanFiles.contains(filePath))
		{
			cleanFiles.append(filePath);
		}
	}
	if (cleanFiles.size() != files.size())
	{
		settings.setValue(
			Settings::FilesList,
			cleanFiles
		);
	}
	settings.endGroup();
	return cleanFiles;
}

void SettingsManager::addRecentFile(
	const QString & fileName
)
{
	QSettings settings;
	settings.beginGroup(
		Settings::RecentFilesGroup
	);
	QStringList files = settings.value(
		Settings::FilesList
	).toStringList();

	files.removeAll(fileName);
	files.prepend(fileName);

	while (files.size() > MAX_RECENT_FILES)
	{
		files.removeLast();
	}

	settings.setValue(
		Settings::FilesList,
		files
	);
	settings.endGroup();
}

void SettingsManager::setRecentFiles(
	const QStringList & files
)
{
	QSettings settings;
	settings.beginGroup(
		Settings::RecentFilesGroup
	);
	settings.setValue(
		Settings::FilesList,
		files
	);
	settings.endGroup();
}

void SettingsManager::saveDocumentState(
	const QString & filePath,
	int page,
	double zoom
)
{
	if (filePath.isEmpty())
	{
		return;
	}

	QSettings settings;
	settings.beginGroup(
		Settings::LastPageGroup
	);
	settings.setValue(
		filePath,
		page
	);
	settings.endGroup();

	settings.beginGroup(
		Settings::ZoomGroup
	);
	settings.setValue(
		filePath,
		zoom
	);
	settings.endGroup();
}

bool SettingsManager::restoreDocumentState(
	const QString & filePath,
	int & page,
	double & zoom
)
{
	if (filePath.isEmpty())
	{
		return false;
	}

	QSettings settings;
	settings.beginGroup(
		Settings::LastPageGroup
	);
	if (!settings.contains(filePath))
	{
		settings.endGroup();
		return false;
	}

	page = settings.value(
		filePath,
		0
	).toInt();
	settings.endGroup();

	settings.beginGroup(
		Settings::ZoomGroup
	);
	zoom = settings.value(
		filePath,
		1.0
	).toDouble();
	settings.endGroup();

	return true;
}
