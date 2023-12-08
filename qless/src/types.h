/*
 * @author rayzhang
 * @desc: 简易的qss预处理器
 */
#pragma once
#include <QString>
#include <QVariant>

namespace ray
{

struct Property
{
    QString name;
    QString value;
};

struct Selector
{
    QString name;
    QVector<Property> props;
    QVector<std::shared_ptr<Selector>> children;
    std::weak_ptr<Selector> parent;
};

using StyleSheets = QVector<std::shared_ptr<Selector>>;
} // namespace ray