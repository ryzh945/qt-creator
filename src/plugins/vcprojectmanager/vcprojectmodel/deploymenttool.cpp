/**************************************************************************
**
** Copyright (c) 2013 Bojan Petrovic
** Copyright (c) 2013 Radovan Zivkvoic
** Contact: http://www.qt-project.org/legal
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
#include "deploymenttool.h"

namespace VcProjectManager {
namespace Internal {

DeploymentTool::DeploymentTool()
{
}

DeploymentTool::DeploymentTool(const DeploymentTool &tool)
{
    m_anyAttribute = tool.m_anyAttribute;
}

DeploymentTool &DeploymentTool::operator=(const DeploymentTool &tool)
{
    if (this != &tool)
        m_anyAttribute = tool.m_anyAttribute;
    return *this;
}

DeploymentTool::~DeploymentTool()
{
}

void DeploymentTool::processNode(const QDomNode &node)
{
    processNodeAttributes(node.toElement());
}

VcNodeWidget *DeploymentTool::createSettingsWidget()
{
    return 0;
}

QDomNode DeploymentTool::toXMLDomNode(QDomDocument &domXMLDocument) const
{
    QDomElement toolNode = domXMLDocument.createElement(QLatin1String("DeploymentTool"));

    QHashIterator<QString, QString> it(m_anyAttribute);

    while (it.hasNext()) {
        it.next();
        toolNode.setAttribute(it.key(), it.value());
    }

    return toolNode;
}

QString DeploymentTool::attributeValue(const QString &attributeName) const
{
    return m_anyAttribute.value(attributeName);
}

void DeploymentTool::setAttribute(const QString &attributeName, const QString &attributeValue)
{
    m_anyAttribute.insert(attributeName, attributeValue);
}

void DeploymentTool::clearAttribute(const QString &attributeName)
{
    if (m_anyAttribute.contains(attributeName))
        m_anyAttribute.insert(attributeName, QString());
}

void DeploymentTool::removeAttribute(const QString &attributeName)
{
    m_anyAttribute.remove(attributeName);
}

void DeploymentTool::processNodeAttributes(const QDomElement &element)
{
    QDomNamedNodeMap namedNodeMap = element.attributes();

    for (int i = 0; i < namedNodeMap.size(); ++i) {
        QDomNode domNode = namedNodeMap.item(i);

        if (domNode.nodeType() == QDomNode::AttributeNode) {
            QDomAttr domElement = domNode.toAttr();
            m_anyAttribute.insert(domElement.name(), domElement.value());
        }
    }
}

} // namespace Internal
} // namespace VcProjectManager