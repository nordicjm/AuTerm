/******************************************************************************
** Copyright (C) 2023-2024 Jamie M.
**
** Project: AuTerm
**
** Module:  plugin_logger.h
**
** Notes:
**
** License: This program is free software: you can redistribute it and/or
**          modify it under the terms of the GNU General Public License as
**          published by the Free Software Foundation, version 3.
**
**          This program is distributed in the hope that it will be useful,
**          but WITHOUT ANY WARRANTY; without even the implied warranty of
**          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**          GNU General Public License for more details.
**
**          You should have received a copy of the GNU General Public License
**          along with this program.  If not, see http://www.gnu.org/licenses/
**
*******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QTime>
#include "plugin_logger.h"
#include "ui_plugin_logger.h"

/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
plugin_logger::plugin_logger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plugin_logger)
{
    ui->setupUi(this);

    //Set checkbox text colour to match that of the conditions
    QPalette palette = ui->check_error->palette();
    palette.setColor(QPalette::WindowText, error_text_colour);
    ui->check_error->setPalette(palette);
    palette.setColor(QPalette::WindowText, warning_text_colour);
    ui->check_warning->setPalette(palette);
    palette.setColor(QPalette::WindowText, information_text_colour);
    ui->check_information->setPalette(palette);
    palette.setColor(QPalette::WindowText, debug_text_colour);
    ui->check_debug->setPalette(palette);
}

plugin_logger::~plugin_logger()
{
    disconnect(this, SLOT(log_level_enabled(log_level_types, bool*)));
    disconnect(this, SLOT(log_message(log_level_types,QString,QString)));
    disconnect(this, SLOT(set_enabled(bool)));

    delete ui;
}

void plugin_logger::setup(QMainWindow *main_window)
{
    //Make window on top of terminal
    this->setParent(main_window);
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

const QString plugin_logger::plugin_about()
{
    return "AuTerm logger plugin\r\nCopyright 2023 Jamie M.\r\n\r\nPlugin and application logging system.\r\n\r\nBuilt using Qt " QT_VERSION_STR;
}

bool plugin_logger::plugin_configuration()
{
    if (QMessageBox::question(this, "Enable logger?", "Enable logger and show log window?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        this->show();
    }
    else
    {
        this->hide();
    }

    return true;
}

QWidget *plugin_logger::plugin_widget()
{
    return this;
}

void plugin_logger::log_level_enabled(enum log_level_types type, bool *enabled)
{
    QCheckBox *check = nullptr;

    switch (type)
    {
        case log_level_error:
        {
            check = ui->check_error;
            break;
        }
        case log_level_warning:
        {
            check = ui->check_warning;
            break;
        }
        case log_level_information:
        {
            check = ui->check_information;
            break;
        }
        case log_level_debug:
        {
            check = ui->check_debug;
            break;
        }
        default:
        {
            *enabled = false;
            return;
        }
    }

    *enabled = check->isChecked();
}

void plugin_logger::log_message(enum log_level_types type, QString sender, QString message)
{
    bool log;
    QBrush new_colour;
    QTextCursor text_cursor;
    QTextCharFormat text_format;

    if (this->isHidden() == true)
    {
        //Only enable logging when window is shown
        return;
    }

    log_level_enabled(type, &log);

    if (log == false)
    {
        return;
    }

    switch (type)
    {
        case log_level_error:
        {
            new_colour = QBrush(error_text_colour);
            break;
        }
        case log_level_warning:
        {
            new_colour = QBrush(warning_text_colour);
            break;
        }
        case log_level_information:
        {
            new_colour = QBrush(information_text_colour);
            break;
        }
        case log_level_debug:
        {
            new_colour = QBrush(debug_text_colour);
            break;
        }
        default:
        {
            return;
        }
    }

    text_cursor = ui->edit_log->textCursor();
    text_format = text_cursor.charFormat();
    text_format.setForeground(new_colour);
    text_cursor.setCharFormat(text_format);

    ui->edit_log->setTextCursor(text_cursor);
    ui->edit_log->appendPlainText(QString("[%1] %2: %3").arg(QTime::currentTime().toString(), sender, message));
}

void plugin_logger::on_btn_clear_clicked()
{
    ui->edit_log->clear();
}

void plugin_logger::on_btn_copy_clicked()
{
    QApplication::clipboard()->setText(ui->edit_log->toPlainText());
}

void plugin_logger::set_enabled(bool enabled)
{
    if (enabled == true)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

AutPlugin::PluginType plugin_logger::plugin_type()
{
    return AutPlugin::Feature;
}

QObject *plugin_logger::plugin_object()
{
    return this;
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/
