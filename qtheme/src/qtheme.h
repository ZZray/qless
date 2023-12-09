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
#define LIBQLESS_API // GCC下不需要特殊处理
#endif

#include <QVector>
#include <QMap>
#include <QString>
#include <QVariant>
namespace ray
{
class LIBQLESS_API QTheme
{
    using ThemeData = QMap<QString, QVector<QString>>;

public:
    /**
     * \brief 添加主题
     * \param name 主题名
     * \param styleFile 样式表文件路径
     * \return 文件不存在将会添加失败
     */
    bool addTheme(const QString& name, const QString& styleFile);
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
    /**
     * \brief 返回内置变量集合的引用。内置变量会在解析样式表的时候使用
     * \return 内置变量集合
     *
     * \desc 比如，可以在配置文件中存储一个变量@primaryColor，程序运行先读取该变量，设置到内置变量，然后在样式表中使用该变量，
     * 当修改了配置文件后，调用reload()方法，会重新解析样式表，然后更新样式表, 以达到切换主题样式的目的。
     */
    QVariantMap& buildInVariables();

    //
    // QString primaryColor() const;
    // QString setPrimaryColor(const QString& color);
    //

private:
    // 主题
    QString _currentTheme;
    ThemeData _themes;
    QVariantMap _themeVariables;
    QVariantMap _buildInVariables;
};
} // namespace ray
