#include "qtheme.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStyle>

#include "../../qless/src/parser.h"
namespace ray
{
QTheme* QTheme::instance()
{
    static auto theme = new QTheme;
    return theme;
}

bool QTheme::addTheme(const QString& name, const QString& styleFile)
{
    if (QFile::exists(styleFile)) {
        _themes[name].push_back(styleFile);
        return true;
    }
    return false;
}

QTheme::QTheme() = default;

bool QTheme::setCurrentTheme(const QString& name)
{
    if (!_themes.contains(name)) {
        return false;
    }
    _currentTheme = name;
    qless::Parser parser(_themes[name]);
    const QString styleSheet = parser.parse();
    _themeVariables          = parser.variables();
    if (!styleSheet.isEmpty()) {
        qApp->style()->unpolish(qApp);
        qApp->setStyleSheet(styleSheet);
        qApp->style()->polish(qApp);
        return true;
    }
    else {
        qDebug() << "theme parse error " << name;
    }
    return false;
}

QString QTheme::currentTheme() const
{
    return _currentTheme;
}

QStringList QTheme::themes() const
{
    return _themes.keys();
}

void QTheme::reload()
{
    setCurrentTheme(_currentTheme);
}

QVariant QTheme::variable(QString name, const QVariant& defaultValue) const
{
    if (!name.startsWith("@")) {
        name = "@" + name;
    }
    return _themeVariables.value(name, defaultValue);
}

QString QTheme::primaryColor() const
{
    return variable("@primaryColor", "#2453A4").toString();
}

QString QTheme::setPrimaryColor(const QString& color)
{
    const QString themeFile = QString("assets/styles/%1/theme.less").arg(_currentTheme);
    const QString content   = QString("@primaryColor:%1;").arg(color);

    // 写入文件
    QFile file(themeFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
        setCurrentTheme(_currentTheme);
    }
    else {
        // 文件打开失败，进行错误处理
        // ...
    }
    return color;
}

} // namespace ray
