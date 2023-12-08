/*
 * @author rayzhang
 * @desc: qless parser
 */
module;
#include <functional>
#include <QMap>
#include <QDebug>
#include <QColor>
#include <QVariantMap>
export module qless.buildin;
export namespace buildin
{
// buildin 内置函数
struct NativeFunction
{
    std::function<QVariant(const QStringList& arguments)> callback;
    // std::function<QVariant(const QVariantList& arguments)> callback;
    //  参数个数
    int arity = -1;
};
class BuildInFunction
{
public:
    BuildInFunction() { init(); }
    bool contains(const QString& name) const { return _functions.contains(name); }
    NativeFunction get(const QString& name) const { return _functions.value(name); }
    template <typename... Args>
    QVariant exec(const Args&... args)
    { }
    void add(const QString& name, const NativeFunction& fn) { _functions.insert(name, fn); }

private:
    void init()
    {
        // 内置函数列表
        add("fade",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "fade function error: " << arguments;
                                   return "";
                               }
                               const auto& color = arguments.at(0);
                               const auto& alpha = arguments.at(1).chopped(1).toInt();
                               QColor c(color);
                               c.setAlpha(alpha);
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});

        add("lighten",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "lighten function error: " << arguments;
                                   return "";
                               }
                               const auto& color  = arguments.at(0);
                               const auto& amount = arguments.at(1).chopped(1).toDouble();
                               QColor c(color);
                               c = c.lighter(int(amount * 100.0));
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});
        add("darken",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "darken function error: " << arguments;
                                   return "";
                               }
                               const auto& color  = arguments.at(0);
                               const auto& amount = arguments.at(1).chopped(1).toDouble();
                               QColor c(color);
                               c = c.darker(int(amount * 100.0));
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});

        // mix() 函数可以将两个颜色混合在一起，生成一个介于两个颜色之间的新颜色
        add("mix",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 3) {
                                   qDebug() << "mix function error: " << arguments;
                                   return "";
                               }
                               const auto& color1 = arguments.at(0);
                               const auto& color2 = arguments.at(1);
                               const auto& weight = arguments.at(2).chopped(1).toDouble();
                               QColor c1(color1);
                               QColor c2(color2);
                               QColor mixColor = c1.toHsv();
                               mixColor.setHsvF(mixColor.hueF(), mixColor.saturationF() * (1.0 - weight) + c2.saturationF() * weight, mixColor.valueF() * (1.0 - weight) + c2.valueF() * weight);
                               return mixColor.name(QColor::HexArgb);
                           },
                           .arity = 3});
        // saturate() 函数可以将颜色的饱和度增加一定的量
        add("saturate",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "saturate function error: " << arguments;
                                   return "";
                               }
                               const auto& color  = arguments.at(0);
                               const auto& amount = arguments.at(1).chopped(1).toDouble();
                               QColor c(color);
                               c = c.toHsv();
                               c.setHsvF(c.hueF(), c.saturationF() + amount / 100.0, c.valueF());
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});
        // desaturate() 函数可以将颜色的饱和度减少一定的量
        add("desaturate",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "desaturate function error: " << arguments;
                                   return "";
                               }
                               const auto& color  = arguments.at(0);
                               const auto& amount = arguments.at(1).chopped(1).toDouble();
                               QColor c(color);
                               c = c.toHsv();
                               c.setHsvF(c.hueF(), c.saturationF() - amount / 100.0, c.valueF());
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});
        // invert() 函数可以将颜色的亮度值取反。如果原来的颜色的亮度值为 0.5，那么取反后的亮度值就是 1 - 0.5 = 0.5
        add("invert",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 1) {
                                   qDebug() << "invert function error: " << arguments;
                                   return "";
                               }
                               const auto& color = arguments.at(0);
                               QColor c(color);
                               c = QColor(255 - c.red(), 255 - c.green(), 255 - c.blue(), c.alpha());
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 1});
        // grayscale() 函数可以将颜色转换为灰度色。
        add("grayscale",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 1) {
                                   qDebug() << "grayscale function error: " << arguments;
                                   return "";
                               }
                               const auto& color = arguments.at(0);
                               QColor c(color);
                               int gray = qRound(0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue());
                               c        = QColor(gray, gray, gray, c.alpha());
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 1});
        // spin() 函数可以将颜色的色相旋转一定的角度
        add("spin",
            NativeFunction{.callback = [](const QStringList& arguments) -> QVariant {
                               if (arguments.size() != 2) {
                                   qDebug() << "spin function error: " << arguments;
                                   return "";
                               }
                               const auto& color = arguments.at(0);
                               const auto& angle = arguments.at(1).chopped(3).toDouble();
                               QColor c(color);
                               c = c.toHsv();
                               c.setHsvF(fmod(c.hueF() + angle / 360.0, 1.0), c.saturationF(), c.valueF());
                               return c.name(QColor::HexArgb);
                           },
                           .arity = 2});
    }

private:
    // 内置函数
    QMap<QString, NativeFunction> _functions;
};
export auto functions = std::make_shared<BuildInFunction>();
}; // namespace buildin