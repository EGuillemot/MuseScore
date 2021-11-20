/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "generalpreferencesmodel.h"

#include "log.h"

using namespace mu::appshell;
using namespace mu::languages;

GeneralPreferencesModel::GeneralPreferencesModel(QObject* parent)
    : QObject(parent)
{
}

void GeneralPreferencesModel::load()
{
    languagesConfiguration()->currentLanguageCode().ch.onReceive(this, [this](const QString& languageCode) {
        emit currentLanguageCodeChanged(languageCode);
    });

    languagesService()->languages().ch.onReceive(this, [this](const LanguagesHash&) {
        emit languagesChanged(languages());
    });

    projectConfiguration()->autoSaveEnabledChanged().onReceive(this, [this](bool enabled) {
        emit autoSaveEnabledChanged(enabled);
    });

    projectConfiguration()->autoSaveIntervalChanged().onReceive(this, [this](int minutes) {
        emit autoSaveIntervalChanged(minutes);
    });

    telemetryConfiguration()->isTelemetryAllowed().ch.onReceive(this, [this](bool) {
        emit isTelemetryAllowedChanged(isTelemetryAllowed());
    });
}

void GeneralPreferencesModel::openUpdateTranslationsPage()
{
    interactive()->open("musescore://home?section=add-ons&subSection=languages");
}

QVariantList GeneralPreferencesModel::languages() const
{
    ValCh<LanguagesHash> languages = languagesService()->languages();
    QList<Language> languageList = languages.val.values();

    QVariantList result;

    for (const Language& language: languageList) {
        if (language.status == LanguageStatus::Status::NoInstalled
            || language.status == LanguageStatus::Status::Undefined) {
            continue;
        }

        QVariantMap languageObj;
        languageObj["code"] = language.code;
        languageObj["name"] = language.name;
        result << languageObj;
    }

    std::sort(result.begin(), result.end(), [](const QVariant& l, const QVariant& r) {
        return l.toMap().value("code").toString() < r.toMap().value("code").toString();
    });

    return result;
}

QString GeneralPreferencesModel::currentLanguageCode() const
{
    return languagesConfiguration()->currentLanguageCode().val;
}

QStringList GeneralPreferencesModel::keyboardLayouts() const
{
    NOT_IMPLEMENTED;
    return { "US-QWERTY", "UK-QWERTY", "QWERTZ", "AZERTY" };
}

QString GeneralPreferencesModel::currentKeyboardLayout() const
{
    return shortcutsConfiguration()->currentKeyboardLayout();
}

bool GeneralPreferencesModel::isTelemetryAllowed() const
{
    return telemetryConfiguration()->isTelemetryAllowed().val;
}

bool GeneralPreferencesModel::isAutoSaveEnabled() const
{
    return projectConfiguration()->isAutoSaveEnabled();
}

int GeneralPreferencesModel::autoSaveInterval() const
{
    return projectConfiguration()->autoSaveIntervalMinutes();
}

bool GeneralPreferencesModel::isOSCRemoteControl() const
{
    return false;
}

int GeneralPreferencesModel::oscPort() const
{
    return 0;
}

void GeneralPreferencesModel::setCurrentLanguageCode(const QString& currentLanguageCode)
{
    if (currentLanguageCode == this->currentLanguageCode()) {
        return;
    }

    languagesConfiguration()->setCurrentLanguageCode(currentLanguageCode);
    emit currentLanguageCodeChanged(currentLanguageCode);
}

void GeneralPreferencesModel::setCurrentKeyboardLayout(const QString& keyboardLayout)
{
    if (keyboardLayout == this->currentKeyboardLayout()) {
        return;
    }

    shortcutsConfiguration()->setCurrentKeyboardLayout(keyboardLayout);
    emit currentKeyboardLayoutChanged();
}

void GeneralPreferencesModel::setIsTelemetryAllowed(bool isTelemetryAllowed)
{
    if (isTelemetryAllowed == this->isTelemetryAllowed()) {
        return;
    }

    telemetryConfiguration()->setIsTelemetryAllowed(isTelemetryAllowed);
    emit isTelemetryAllowedChanged(isTelemetryAllowed);
}

void GeneralPreferencesModel::setAutoSaveEnabled(bool enabled)
{
    if (enabled == isAutoSaveEnabled()) {
        return;
    }

    projectConfiguration()->setAutoSaveEnabled(enabled);
    emit autoSaveEnabledChanged(enabled);
}

void GeneralPreferencesModel::setAutoSaveInterval(int minutes)
{
    if (minutes == autoSaveInterval()) {
        return;
    }

    projectConfiguration()->setAutoSaveInterval(minutes);
    emit autoSaveIntervalChanged(minutes);
}

void GeneralPreferencesModel::setIsOSCRemoteControl(bool isOSCRemoteControl)
{
    NOT_IMPLEMENTED;
    emit isOSCRemoteControlChanged(isOSCRemoteControl);
}

void GeneralPreferencesModel::setOscPort(int oscPort)
{
    NOT_IMPLEMENTED;
    emit oscPortChanged(oscPort);
}
