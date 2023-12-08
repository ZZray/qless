/**
 * @author: rayzhang
 * @desc: theme manager
 * @todo:编写一个轻量 tiny-qtscss-parser 用于减少模板样式代码
 */
#pragma once

#if defined(_MSC_VER)            // 在Microsoft的编译器中
#if defined(LIBQLESS_API_EXPORT) // 提供一个 LIBQLESS_EXPORT 宏。当需要导出时，在编译工程中定义它
#define LIBQLESS_API __declspec(dllexport)
#else
#define LIBQLESS_API __declspec(dllimport)
#endif
#else
#define MYLIBRARY_API // GCC下不需要特殊处理
#endif

#include <QVector>
#include <QMap>
#include <QString>
#include <QVariant>
namespace ray
{
class LIBQLESS_API QTheme
{
    using ThemeData = QMap<QString, QString>;

public:
    static QTheme* instance();
    /**
     * \brief 添加主题
     * \param name 主题名
     * \param styleFile 样式表文件路径
     * \param variableFile 样式表依赖的变量路径
     * \return 文件不存在将会添加失败
     */
    bool addTheme(const QString& name, const QString& styleFile);
    /**
     * \brief 附加样式到指定的主题，如果指定的主题不存在则会创建一个
     * \param name 主题名
     * \param styleFile 样式表文件路径
     * \param variableFile 样式表依赖的文件路径
     * \return 文件不存在将会添加失败
     */
    // bool append(const QString& name, const QString& styleFile);

    // 设置当前主题
    bool setCurrentTheme(const QString& name);
    // 获取当前主题
    QString currentTheme() const;
    // 获取主题
    QStringList themes() const;
    // 刷新主题
    void reload();
    // 获取主题变量
    QVariant variable(QString name, const QVariant& defaultValue = {}) const;
    QString primaryColor() const;
    QString setPrimaryColor(const QString& color);

private:
    QTheme();

private:
    // 主题
    QString _currentTheme;
    ThemeData _themes;
    QVariantMap _themeVariables;
};
} // namespace ray
