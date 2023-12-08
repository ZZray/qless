/*
 * @author rayzhang
 * @desc: 简易的qss预处理器
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

#include <QString>
#include <QVariantMap>
#include "types.h"

namespace ray::qless
{
class LIBQLESS_API Parser
{
private:
    QString _filename;
    // 解析的变量
    QVariantMap _variables;
public:
    Parser(QString filename, QVariantMap initVariables = {});
    QString parse();
    QVariantMap variables() const { return _variables; }
    QVariant variable(QString name, const QVariant& defaultValue) const;

private:
    // 读取样式表文件内容
    StyleSheets parseFile(const QString& styleFile);
    StyleSheets parseImport(QString statement);
    void parseVariable(const QString& statement);
    void applyVariables(QString& statement) const;
    void parseFunctions(QString& statement);

    static QString compile(const StyleSheets &styleSheets);
};
} // namespace ray::qless
