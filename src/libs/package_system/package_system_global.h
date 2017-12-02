#pragma once

#include <QtCore/qglobal.h>

#if defined(PACKAGE_SYSTEM_LIBRARY)
#  define PACKAGE_SYSTEM_EXPORT Q_DECL_EXPORT
#else
#  define PACKAGE_SYSTEM_EXPORT Q_DECL_IMPORT
#endif

