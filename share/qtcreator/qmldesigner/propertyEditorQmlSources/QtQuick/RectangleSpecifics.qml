/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

import QtQuick 2.0
import HelperWidgets 2.0
import QtQuick.Layouts 1.0


Column {
    anchors.left: parent.left
    anchors.right: parent.right

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: "Color"

        ColorEditor {
            id: colorEditor
            color: backendValues.color.value
            onColorChanged: {
                //backendValues.color.value = color
                //Delay setting the color to keep ui responsive
                colorEditorTimer.restart()
            }

            Timer {
                id: colorEditorTimer
                repeat: false
                interval: 100
                onTriggered: {
                    backendValues.color.value = colorEditor.color
                }
            }

        }

    }

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: "Rectangle"

        SectionLayout {
            rows: 2
            Label {
                text: "Border"
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.border_width
                    hasSlider: true
                    Layout.preferredWidth: 80
                }
                ExpandingSpacer {

                }
            }
            Label {
                text: "Radius"
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.radius
                    hasSlider: true
                    Layout.preferredWidth: 80
                }
                ExpandingSpacer {

                }
            }
        }
    }
}