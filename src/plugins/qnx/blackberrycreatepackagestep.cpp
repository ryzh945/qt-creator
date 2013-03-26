/**************************************************************************
**
** Copyright (C) 2011 - 2013 Research In Motion
**
** Contact: Research In Motion (blackberry-qt@qnx.com)
** Contact: KDAB (info@kdab.com)
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "blackberrycreatepackagestep.h"

#include "qnxconstants.h"
#include "blackberrycreatepackagestepconfigwidget.h"
#include "blackberrydeployconfiguration.h"
#include "qnxutils.h"
#include "blackberryqtversion.h"
#include "blackberrydeviceconfiguration.h"
#include "blackberrydeployinformation.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>
#include <qt4projectmanager/qt4buildconfiguration.h>
#include <qt4projectmanager/qt4nodes.h>
#include <qt4projectmanager/qt4project.h>
#include <qtsupport/qtkitinformation.h>
#include <utils/qtcassert.h>

#include <QFile>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
const char PACKAGER_CMD[] = "blackberry-nativepackager";

const char QT_INSTALL_LIBS[]        = "QT_INSTALL_LIBS";
const char QT_INSTALL_LIBS_VAR[]    = "%QT_INSTALL_LIBS%";
const char QT_INSTALL_PLUGINS[]     = "QT_INSTALL_PLUGINS";
const char QT_INSTALL_PLUGINS_VAR[] = "%QT_INSTALL_PLUGINS%";
const char QT_INSTALL_IMPORTS[]     = "QT_INSTALL_IMPORTS";
const char QT_INSTALL_IMPORTS_VAR[] = "%QT_INSTALL_IMPORTS%";
const char QT_INSTALL_QML[]         = "QT_INSTALL_QML";
const char QT_INSTALL_QML_VAR[]     = "%QT_INSTALL_QML%";
const char SRC_DIR_VAR[]            = "%SRC_DIR%";
}

BlackBerryCreatePackageStep::BlackBerryCreatePackageStep(ProjectExplorer::BuildStepList *bsl)
    : BlackBerryAbstractDeployStep(bsl, Core::Id(Constants::QNX_CREATE_PACKAGE_BS_ID))
{
    setDisplayName(tr("Create BAR packages"));
}

BlackBerryCreatePackageStep::BlackBerryCreatePackageStep(ProjectExplorer::BuildStepList *bsl,
                                           BlackBerryCreatePackageStep *bs)
    : BlackBerryAbstractDeployStep(bsl, bs)
{
    setDisplayName(tr("Create BAR packages"));
}

bool BlackBerryCreatePackageStep::init()
{
    if (!BlackBerryAbstractDeployStep::init())
        return false;

    const QString packageCmd = target()->activeBuildConfiguration()->environment().searchInPath(QLatin1String(PACKAGER_CMD));
    if (packageCmd.isEmpty()) {
        raiseError(tr("Could not find packager command '%1' in the build environment")
                   .arg(QLatin1String(PACKAGER_CMD)));
        return false;
    }

    BlackBerryDeployConfiguration *deployConfig = qobject_cast<BlackBerryDeployConfiguration *>(deployConfiguration());
    QTC_ASSERT(deployConfig, return false);

    QList<BarPackageDeployInformation> packagesToDeploy = deployConfig->deploymentInfo()->enabledPackages();
    if (packagesToDeploy.isEmpty()) {
        raiseError(tr("No packages enabled for deployment"));
        return false;
    }

    foreach (const BarPackageDeployInformation &info, packagesToDeploy) {
        if (info.appDescriptorPath().isEmpty()) {
            raiseError(tr("Application descriptor file not specified, please check deployment settings"));
            return false;
        }

        if (info.packagePath().isEmpty()) {
            raiseError(tr("No package specified, please check deployment settings"));
            return false;
        }

        const QString buildDir = QFileInfo(info.packagePath()).absolutePath();
        QDir dir(buildDir);
        if (!dir.exists()) {
            if (!dir.mkpath(buildDir)) {
                raiseError(tr("Could not create build directory '%1'").arg(buildDir));
                return false;
            }
        }

        const QString preparedFilePath =  buildDir + QLatin1String("/bar-descriptor-") + project()->displayName() + QLatin1String("-qtc-generated.xml");
        if (!prepareAppDescriptorFile(info.appDescriptorPath(), preparedFilePath))
            // If there is an error, prepareAppDescriptorFile() will raise it
            return false;


        QStringList args;
        args << QLatin1String("-devMode");
        if (!debugToken().isEmpty())
            args << QLatin1String("-debugToken") << QnxUtils::addQuotes(QDir::toNativeSeparators(debugToken()));
        args << QLatin1String("-package") << QnxUtils::addQuotes(QDir::toNativeSeparators(info.packagePath()));
        args << QnxUtils::addQuotes(QDir::toNativeSeparators(preparedFilePath));
        addCommand(packageCmd, args);
    }

    return true;
}

ProjectExplorer::BuildStepConfigWidget *BlackBerryCreatePackageStep::createConfigWidget()
{
    return new BlackBerryCreatePackageStepConfigWidget();
}

QString BlackBerryCreatePackageStep::debugToken() const
{
    BlackBerryDeviceConfiguration::ConstPtr device = BlackBerryDeviceConfiguration::device(target()->kit());
    if (!device)
        return QString();

    return device->debugToken();
}

bool BlackBerryCreatePackageStep::prepareAppDescriptorFile(const QString &appDescriptorPath, const QString &preparedFilePath)
{
    BlackBerryQtVersion *qtVersion = dynamic_cast<BlackBerryQtVersion *>(QtSupport::QtKitInformation::qtVersion(target()->kit()));
    if (!qtVersion) {
        raiseError(tr("Error preparing application descriptor file"));
        return false;
    }

    QFile file(appDescriptorPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        raiseError(tr("Could not open '%1' for reading").arg(appDescriptorPath));
        return false;
    }

    QFile preparedFile(preparedFilePath);

    QByteArray fileContent = file.readAll();

    // Add Warning text
    const QString warningText = QString::fromLatin1("<!-- This file is autogenerated;"
                                                       " any changes will get overwritten if deploying with Qt Creator -->\n<qnx");
    fileContent.replace("<qnx", warningText.toLatin1());

    // Replace Qt path placeholders
    if (fileContent.contains(QT_INSTALL_LIBS_VAR))
        fileContent.replace(QT_INSTALL_LIBS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_LIBS)).toLatin1());
    if (fileContent.contains(QT_INSTALL_PLUGINS_VAR))
        fileContent.replace(QT_INSTALL_PLUGINS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_PLUGINS)).toLatin1());
    if (fileContent.contains(QT_INSTALL_IMPORTS_VAR))
        fileContent.replace(QT_INSTALL_IMPORTS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_IMPORTS)).toLatin1());
    if (fileContent.contains(QT_INSTALL_QML_VAR))
        fileContent.replace(QT_INSTALL_QML_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_QML)).toLatin1());

    //Replace Source path placeholder
    if (fileContent.contains(SRC_DIR_VAR))
        fileContent.replace(SRC_DIR_VAR, QDir::toNativeSeparators(target()->project()->projectDirectory()).toLatin1());

    // Add parameter for QML debugging (if enabled)
    if (target()->activeRunConfiguration()->debuggerAspect()->useQmlDebugger()) {
        if (!fileContent.contains("-qmljsdebugger")) {
            const QString argString = QString::fromLatin1("<arg>-qmljsdebugger=port:%1</arg>\n</qnx>")
                    .arg(target()->activeRunConfiguration()->debuggerAspect()->qmlDebugServerPort());
            fileContent.replace("</qnx>", argString.toLatin1());
        }
    }

    const QString buildDir = target()->activeBuildConfiguration()->buildDirectory();
    if (!preparedFile.open(QIODevice::WriteOnly)) {
        raiseError(tr("Could not create prepared application descriptor file in '%1'").arg(buildDir));
        return false;
    }

    preparedFile.write(fileContent);
    preparedFile.close();

    return true;
}
