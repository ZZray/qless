#include "parser.h"
#include <memory>
#include <QApplication>

#include "types.h"
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
import qless.buildin;
namespace ray::qless
{
Parser::Parser(const QString& filename)
    : _filename(filename)
{ }

QString Parser::parse()
{
    const auto sheets = parseFile(_filename);
    // todo: 生成qss
    return compile(sheets);
}

void Parser::parseFunctions(QString& statement)
{
    // 解析其中的fade函数，并执行 background-color: fade(@primaryColor, 0.8);
    // 1. 提取函数名和参数
    const QRegularExpression functionRegex(R"((\w+)\(([^)]+)\))");
    const QRegularExpressionMatch functionMatch = functionRegex.match(statement);
    const QString functionName                  = functionMatch.captured(1);
    if (functionName.isEmpty() || !buildin::functions->contains(functionName)) {
        return;
    }
    const QString argsString = functionMatch.captured(2);

    // 2. 分割参数
    const QStringList argsList = argsString.split(QRegularExpression("\\s*,\\s*"));

    // 3. 执行函数
    const buildin::NativeFunction& function = buildin::functions->get(functionName);
    if (function.arity == argsList.size()) {
        const QVariant result = function.callback(argsList);

        // 4. 替换结果
        statement.replace(functionMatch.capturedStart(0), functionMatch.capturedLength(0), result.toString());
    }
}

QString Parser::compile(const StyleSheets& styleSheets)
{
    QString qss;
    const auto compileSelector = [&](this auto self, const std::shared_ptr<Selector>& selector) -> void {
        auto name = selector->name;
        if (const auto parent = selector->parent.lock()) {
            // 如果parent是逗号分割的
            const bool startWithAnd = name.startsWith("&");
            if (startWithAnd) {
                name.remove(0, 1);
            }
            QString newName;
            const auto parentNames = parent->name.split(",");
            for (int i = 0; i < parentNames.size(); ++i) {
                const auto parentName = parentNames.at(i).trimmed();
                const QString sep     = i == parentNames.size() - 1 ? " " : ",\n";
                if (startWithAnd) {
                    newName += parentName + name + sep;
                }
                else {
                    newName += parentName + " " + name + sep;
                }
            }
            name = newName;
        }
        qss += name.trimmed() + "{\n";
        for (const auto& prop : selector->props) {
            qss += prop.name + ":" + prop.value + ";\n";
        }
        qss += "}\n";
        for (const auto& child : selector->children) {
            self(child);
        }
    };
    for (const auto selector : styleSheets) {
        compileSelector(selector);
    }
    return qss;
}

QVariant Parser::variable(QString name, const QVariant& defaultValue) const
{
    if (!name.startsWith("@")) {
        name = "@" + name;
    }
    return _variables.value(name, defaultValue);
}
/**
 * 每一行作为一个解析单元, 为了方便解析，所以格式很严格，限制比较大，但是简单。
 * 1. 定义 @import 语句是有顺序的，先导入才能使用
 * 2. 定义 @variable 语句是有顺序的，先定义才能使用，
 * 所有文件解析完后，变量会存入到_variables，所以后解析的变量会覆盖之前的同名变量。没有作用域(env)。
 */
StyleSheets Parser::parseFile(const QString& styleFile)
{
    try {
        QFile file(styleFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            StyleSheets styleSheets{};
            StyleSheets nestedStyleSheets{};
            std::shared_ptr<Selector> currentSelector = nullptr;
            // todo: 逐字符读取token以更灵活的解析
            QString selectorName;
            while (!file.atEnd()) {
                // 读取一行，并过滤空白字符和注释
                QString statement = QString::fromUtf8(file.readLine().trimmed());
                if (!statement.isEmpty() && !statement.startsWith("//")) {
                    // 移除注释
                    statement = statement.remove(QRegularExpression("//.*"));
                    // 解析导入
                    if (statement.startsWith("@import")) {
                        styleSheets += parseImport(statement);
                        continue;
                    }
                    //
                    // 以这个开头的就是变量定义
                    if (statement.startsWith("@")) {
                        parseVariable(statement);
                        continue;
                    }
                    // 变量替换
                    applyVariables(statement);
                    // 函数
                    parseFunctions(statement);
                    // mixin qss用处不大，暂时不解析
                    if (statement.startsWith("@mixin")) {
                        // TODO
                        continue;
                    }
                    // styleSheet += statement + "\n";
                    //
                    if (statement.contains("{")) {
                        if (currentSelector) {
                            nestedStyleSheets.push_back(currentSelector);
                        }
                        const auto parent     = nestedStyleSheets.isEmpty() ? nullptr : nestedStyleSheets.back();
                        currentSelector       = std::make_shared<Selector>();
                        currentSelector->name = selectorName + statement.left(statement.indexOf("{")).trimmed();
                        selectorName.clear();
                        if (parent) {
                            currentSelector->parent = parent;
                            parent->children.push_back(currentSelector);
                        }
                    }
                    // handle closing bracket
                    else if (statement.contains("}")) {
                        const auto parent = currentSelector->parent.lock();
                        if (!parent) {
                            styleSheets.push_back(currentSelector);
                            nestedStyleSheets.clear();
                            selectorName.clear();
                        }
                        currentSelector = parent;
                    }
                    else if (statement.endsWith(",")) {
                        selectorName += statement;
                    }
                    // handle property
                    else {
                        const auto& kv = statement.split(':');
                        if (kv.size() < 2) {
                            qDebug() << "property statement error: " << statement;
                            // startWidth("@include")
                            continue;
                        }
                        const auto key = kv.at(0).trimmed();
                        auto value     = kv.at(1).trimmed();
                        value.chop(1);
                        currentSelector->props.push_back({key, value});
                    }
                }
            }
            file.close();
            return styleSheets;
        }
        else {
            qDebug() << "file parse error: " << styleFile << " " << file.errorString();
        }
    }
    catch (std::exception& e) {
        qDebug() << "error in theme parser : " << e.what();
    }
    return {};
}

StyleSheets Parser::parseImport(QString statement)
{
    // 归解析导入
    // 提取 @import "filename"中的 filename
    const auto strs = statement.remove("@import").trimmed().split('"');
    if (strs.size() < 2) {
        qDebug() << "import statement error: " << statement;
    }
    auto filename = strs.at(1);
    if (filename.startsWith("./")) {
        filename.remove("./");
    }
    if (!filename.contains("/")) {
        filename = QFileInfo(_filename).absolutePath() + "/" + filename;
    }
    return parseFile(filename);
}
void Parser::parseVariable(const QString& statement)
{
    const auto& kv = statement.split(':');
    if (kv.size() < 2) {
        qDebug() << "variable statement error: " << statement;
        return;
    }
    const auto key = kv.at(0).trimmed();
    auto value     = kv.at(1).trimmed();
    value.chop(1);
    if (value.startsWith("@")) {
        // 变量替换
        value = this->variable(value, value).toString().toUtf8();
    }
    // 判断value类型
    _variables[key] = value;
}

void Parser::applyVariables(QString& statement) const
{
    // const auto& kv = statement.split(':');
    // if (kv.size() < 2) {
    //     return;
    // }
    // const auto key = kv.at(0).trimmed();
    // auto value     = kv.at(1).trimmed();
    // if (key.startsWith("@")) {
    //     // 变量替换
    //     const auto var = this->variable(key, key).toString().toUtf8();
    //     statement.replace(key, var);
    // }
    // if (value.startsWith("@")) {
    //     if (!value.endsWith(";")) {
    //         qDebug() << "warning: " << statement << " value not end with ;";
    //     }
    //     else {
    //         value.chop(1);
    //     }
    //     // 变量替换
    //     const auto var = this->variable(value, value).toString().toUtf8();
    //     statement.replace(value, var);
    // }

    if (statement.contains("@")) {
        for (const auto& name : _variables.keys()) {
            statement.replace(name, _variables.value(name).toString());
        }
    }
}

} // namespace ray::qless
