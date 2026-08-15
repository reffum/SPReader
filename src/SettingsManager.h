#ifndef SPREADER_SETTINGSMANAGER_H
#define SPREADER_SETTINGSMANAGER_H

#include <QStringList>

class SettingsManager
{
public:
	static QString getLastDir();
	static void setLastDir(const QString & dir);

	static QStringList getRecentFiles();

	static void addRecentFile(const QString & fileName);
	static void setRecentFiles(const QStringList & files);

	static void setLastPage(const QString& filePath, int pageNumber);
	static std::optional<int> getLastPage(const QString & filePath);

	static void setZoom(const QString& filePath, double zoom);
	static std::optional<double> getZoom(const QString & filePath);
};

#endif // SPREADER_SETTINGSMANAGER_H
