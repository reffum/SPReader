#include "SettingsManager.h"

#include <qcryptographichash.h>
#include <QDir>

#include "settings_keys.h"
#include <QFileInfo>
#include <QSettings>
#include <qstandardpaths.h>

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

// Return file path of file with bookmarks
QString SettingsManager::getBookmarksFile(const QString & filePath)
{
	QSettings settings;
	settings.beginGroup(Settings::BookmarksGroup);
	QVariant value = settings.value(filePath);

	//
	// Generate new if there isn't the file.
	// The name if <sanitized_stem>_<short_path_hash>.json
	// sanitized_stem - STEM_SIZE left symbols of filePath
	// short_path_hash - HASH_SIZE left of sha256 of fileName absolute path
	//
	if (!value.isValid())
	{
		constexpr int STEM_SIZE = 20;
		constexpr int HASH_SIZE = 12;

		QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		QDir dir(baseDir + "/bookmarks");
		if (!dir.exists())
		{
			bool r = dir.mkpath(".");
			if (!r) qCritical() << QObject::tr("Cannot create directory \"") << baseDir;
			throw SettingsError(QString("Can't create bookmarks directory: %1").arg(dir.absolutePath()));
		}

		QFileInfo fileInfo(filePath);
		QByteArray hash = QCryptographicHash::hash(
			fileInfo.canonicalFilePath().toUtf8(),
			QCryptographicHash::Sha256
			).toHex().left(HASH_SIZE);

		QString safeFileName = fileInfo.completeBaseName().left(STEM_SIZE);
		safeFileName.removeIf(
			[](QChar c) {
				return !c.isLetterOrNumber();
			}
		);

		QString jsonFileName = QString("%1_%2.json").arg(safeFileName, QString::fromLatin1(hash));
		QString fullJsonFileName = dir.filePath(jsonFileName);
		settings.setValue(filePath, fullJsonFileName);

		settings.endGroup();

		return fullJsonFileName;
	}

	settings.endGroup();
	return value.toString();
}