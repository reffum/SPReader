#ifndef SPREADER_SETTINGSMANAGER_H
#define SPREADER_SETTINGSMANAGER_H

#include <QString>
#include <QStringList>

class SettingsManager
{
public:
	static QString getLastDir();
	static void setLastDir(
		const QString & dir
	);

	static QStringList getRecentFiles();
	static void addRecentFile(
		const QString & fileName
	);
	static void setRecentFiles(
		const QStringList & files
	);

	static void saveDocumentState(
		const QString & filePath,
		int page,
		double zoom
	);
	static bool restoreDocumentState(
		const QString & filePath,
		int & page,
		double & zoom
	);
};

#endif // SPREADER_SETTINGSMANAGER_H
