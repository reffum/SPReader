# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.6.0] - 2026-11-01

### Added
- Added Select text and Select image mode for PDF documents.

## [0.5.0] - 2026-11-01

### Added
- Added document bookmarks support (`BookmarksModel` and `BookmarksTreeView`) with JSON persistence.
- Added support for opening documents passed via command-line arguments.
- Added mouse drag scrolling for PDF and DjVu documents (`DraggableScrollArea`, `PdfView`).
- Added `View -> Contents` menu action to toggle the contents panel.

### Refactored
- Extracted unified `DocumentWidget` base class with `PdfWidget` and `DjvuWidget` implementations.
- Centralized application constants in `constants.h`.

## [0.4.0] - 2026-08-16
- Improve DjVu document rendering

## [0.3.0] - 2026-08-07

### Added
- Added DjVu document viewing support (`DjvuDocument`, `DjvuView`, and `DjvuContentsModel`).
- Added support for restoring last opened documents upon launch.
- Added Arch Linux PKGBUILD package definition.
- Added Help -> About dialog displaying application version and credits.
- Added desktop entry and scalable icon install target.

### Changed
- Reorganized and updated configuration settings format to store recent documents and UI state cleanly.
- Updated application organization and application name initialization.
- Improved document handling to open each new document in its own tab.

### Refactored
- Refactored slot naming conventions (renamed non-autoconnected slots for consistency).
- Cleaned up source code includes and directory structure.

## [0.2.0] - 2026-08-01

### Added
- Multi-page document viewing for PDF files.
- Table of Contents navigation view panel.
- Page navigation controls (next page, previous page, page jump).
- Zoom in, zoom out, and reset zoom functionality.
- README documentation and user guide images.

## [0.1.0] - 2026-07-25

### Added
- Initial project scaffold with CMake and Qt6.
- Main application window with menu bar and file open action.
- Basic PDF document viewing using `QPdfView`.
