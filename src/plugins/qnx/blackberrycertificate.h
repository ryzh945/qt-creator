/**************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
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

#ifndef QNX_INTERNAL_BLACKBERRYCERTIFICATE_H
#define QNX_INTERNAL_BLACKBERRYCERTIFICATE_H

#include <QObject>
#include <QString>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

namespace Qnx {
namespace Internal {

class BlackBerryCertificate : public QObject
{
Q_OBJECT

public:
    enum ResultCode {
        Success,
        Busy,
        WrongPassword,
        PasswordTooSmall,
        InvalidOutputFormat,
        Error
    };

    BlackBerryCertificate(const QString &fileName,
            const QString &author = QString(),
            const QString &storePass = QString(),
            QObject *parent = 0);

    void load();
    void store();

    QString fileName() const;
    QString author() const;
    QString id() const;

signals:
    void loaded();
    void stored();
    void finished(int status);

private slots:
    void storeFinished(int status);
    void loadFinished();
    void processError();

private:
    QString command() const;

    QString m_fileName;
    QString m_author;
    QString m_storePass;

    QProcess *m_process;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_BLACKBERRYCERTIFICATE_H
