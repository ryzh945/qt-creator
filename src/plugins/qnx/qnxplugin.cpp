/**************************************************************************
**
** Copyright (C) 2012, 2013 BlackBerry Limited. All rights reserved.
**
** Contact: BlackBerry (qt@blackberry.com)
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

#include "qnxplugin.h"

#include "blackberrydeviceconfigurationfactory.h"
#include "qnxconstants.h"
#include "blackberryqtversionfactory.h"
#include "blackberrydeployconfigurationfactory.h"
#include "blackberrycreatepackagestepfactory.h"
#include "blackberrydeploystepfactory.h"
#include "blackberryrunconfigurationfactory.h"
#include "blackberryruncontrolfactory.h"
#include "qnxdeviceconfigurationfactory.h"
#include "qnxruncontrolfactory.h"
#include "qnxdeploystepfactory.h"
#include "qnxdeployconfigurationfactory.h"
#include "qnxrunconfigurationfactory.h"
#include "qnxqtversionfactory.h"
#include "blackberryndksettingspage.h"
#include "bardescriptoreditorfactory.h"
#include "bardescriptormagicmatcher.h"
#include "blackberrykeyspage.h"
#include "blackberrycheckdebugtokenstepfactory.h"
#include "blackberrydeviceconnectionmanager.h"
#include "blackberryconfigurationmanager.h"
#include "cascadesimport/cascadesimportwizard.h"
#include "qnxtoolchain.h"


#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/taskhub.h>
#include <projectexplorer/kitmanager.h>

#include <QtPlugin>

using namespace Qnx::Internal;

QNXPlugin::QNXPlugin()
{
}

QNXPlugin::~QNXPlugin()
{
    delete BlackBerryDeviceConnectionManager::instance();
    delete &BlackBerryConfigurationManager::instance();
}

bool QNXPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Handles BlackBerry
    addAutoReleasedObject(new BlackBerryQtVersionFactory);
    addAutoReleasedObject(new BlackBerryDeployConfigurationFactory);
    addAutoReleasedObject(new BlackBerryDeviceConfigurationFactory);
    addAutoReleasedObject(new BlackBerryCreatePackageStepFactory);
    addAutoReleasedObject(new BlackBerryDeployStepFactory);
    addAutoReleasedObject(new BlackBerryRunConfigurationFactory);
    addAutoReleasedObject(new BlackBerryRunControlFactory);
    addAutoReleasedObject(new BlackBerryNDKSettingsPage);
    addAutoReleasedObject(new BlackBerryKeysPage);
    addAutoReleasedObject(new BlackBerryCheckDebugTokenStepFactory);
    addAutoReleasedObject(new CascadesImportWizard);
    BlackBerryDeviceConnectionManager::instance()->initialize();

    // Handles QNX
    addAutoReleasedObject(new QnxQtVersionFactory);
    addAutoReleasedObject(new QnxDeviceConfigurationFactory);
    addAutoReleasedObject(new QnxRunControlFactory);
    addAutoReleasedObject(new QnxDeployStepFactory);
    addAutoReleasedObject(new QnxDeployConfigurationFactory);
    addAutoReleasedObject(new QnxRunConfigurationFactory);

    // Handle Qcc Compiler
    addAutoReleasedObject(new QnxToolChainFactory);

    // bar-descriptor.xml editor
    Core::MimeGlobPattern barDescriptorGlobPattern(QLatin1String("*.xml"), Core::MimeGlobPattern::MinWeight + 1);
    Core::MimeType barDescriptorMimeType;
    barDescriptorMimeType.setType(QLatin1String(Constants::QNX_BAR_DESCRIPTOR_MIME_TYPE));
    barDescriptorMimeType.setComment(tr("Bar descriptor file (BlackBerry)"));
    barDescriptorMimeType.setGlobPatterns(QList<Core::MimeGlobPattern>() << barDescriptorGlobPattern);
    barDescriptorMimeType.addMagicMatcher(QSharedPointer<Core::IMagicMatcher>(new BarDescriptorMagicMatcher));
    barDescriptorMimeType.setSubClassesOf(QStringList() << QLatin1String("application/xml"));

    if (!Core::MimeDatabase::addMimeType(barDescriptorMimeType)) {
        *errorString = tr("Could not add mime-type for bar-descriptor.xml editor.");
        return false;
    }
    addAutoReleasedObject(new BarDescriptorEditorFactory);

    connect(ProjectExplorer::KitManager::instance(), SIGNAL(kitsLoaded()), &BlackBerryConfigurationManager::instance(), SLOT(loadSettings()));

    return true;
}

void QNXPlugin::extensionsInitialized()
{
    ProjectExplorer::TaskHub::addCategory(Constants::QNX_TASK_CATEGORY_BARDESCRIPTOR,
                                          tr("Bar Descriptor"));
}

ExtensionSystem::IPlugin::ShutdownFlag QNXPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN2(QNX, QNXPlugin)
