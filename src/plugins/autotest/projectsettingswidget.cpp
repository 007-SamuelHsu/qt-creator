/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "autotestplugin.h"
#include "projectsettingswidget.h"
#include "testframeworkmanager.h"
#include "testprojectsettings.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>

namespace Autotest {
namespace Internal {

enum ItemDataRole  { FrameworkIdRole = Qt::UserRole + 1 };

static QSpacerItem *createSpacer(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
    return new QSpacerItem(20, 10, horizontal, vertical);
}

ProjectTestSettingsWidget::ProjectTestSettingsWidget(ProjectExplorer::Project *project,
                                                     QWidget *parent)
    : QWidget(parent)
    , m_projectSettings(AutotestPlugin::projectSettings(project))
{
    auto verticalLayout = new QVBoxLayout(this);
    m_useGlobalSettings = new QComboBox;
    m_useGlobalSettings->addItem(tr("Global"));
    m_useGlobalSettings->addItem(tr("Custom"));

    auto generalWidget = new QWidget;
    auto groupBoxLayout = new QVBoxLayout;
    m_activeFrameworks = new QTreeWidget;
    m_activeFrameworks->setHeaderHidden(true);
    m_activeFrameworks->setRootIsDecorated(false);
    groupBoxLayout->addWidget(new QLabel(tr("Active frameworks:")));
    groupBoxLayout->addWidget(m_activeFrameworks);
    m_runAfterBuild = new QCheckBox(tr("Automatically run after build"));
    m_runAfterBuild->setChecked(m_projectSettings->runAfterBuild());
    groupBoxLayout->addWidget(m_runAfterBuild);
    generalWidget->setLayout(groupBoxLayout);

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_useGlobalSettings);
    horizontalLayout->addItem(createSpacer(QSizePolicy::Expanding, QSizePolicy::Minimum));
    verticalLayout->addLayout(horizontalLayout);
    horizontalLayout = new QHBoxLayout;
    verticalLayout->addItem(createSpacer(QSizePolicy::Minimum, QSizePolicy::Fixed));
    horizontalLayout->addWidget(generalWidget);
    horizontalLayout->addItem(createSpacer(QSizePolicy::Expanding, QSizePolicy::Minimum));
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addItem(createSpacer(QSizePolicy::Minimum, QSizePolicy::Expanding));

    m_useGlobalSettings->setCurrentIndex(m_projectSettings->useGlobalSettings() ? 0 : 1);
    generalWidget->setDisabled(m_projectSettings->useGlobalSettings());

    populateFrameworks(m_projectSettings->activeFrameworks());

    connect(m_useGlobalSettings, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, generalWidget](int index) {
        generalWidget->setEnabled(index != 0);
        m_projectSettings->setUseGlobalSettings(index == 0);
    });
    connect(m_activeFrameworks, &QTreeWidget::itemChanged,
            this, &ProjectTestSettingsWidget::onActiveFrameworkChanged);
    connect(m_runAfterBuild, &QCheckBox::toggled,
            m_projectSettings, &TestProjectSettings::setRunAfterBuild);
}

void ProjectTestSettingsWidget::populateFrameworks(const QMap<Core::Id, bool> &frameworks)
{
    TestFrameworkManager *frameworkManager = TestFrameworkManager::instance();
    auto end = frameworks.cend();
    for (auto it = frameworks.cbegin(); it != end; ++it) {
        auto *item = new QTreeWidgetItem(m_activeFrameworks,
                                         QStringList(frameworkManager->frameworkNameForId(it.key())));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setCheckState(0, it.value() ? Qt::Checked : Qt::Unchecked);
        item->setData(0, FrameworkIdRole, it.key().toSetting());
    }
}

void ProjectTestSettingsWidget::onActiveFrameworkChanged(QTreeWidgetItem *item, int column)
{
    auto id = Core::Id::fromSetting(item->data(column, FrameworkIdRole));
    m_projectSettings->activateFramework(id, item->data(0, Qt::CheckStateRole) == Qt::Checked);
}

} // namespace Internal
} // namespace Autotest
