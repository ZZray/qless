# qless
使得QSS 支持导入，变量，嵌套，支持内嵌函数，不支持自定义函数，mixin。由于qss的样式语法很少，没有必要支持很多特性。

简易版，为了方便解析，对格式有要求，见示例。

其中

1. 大括号`{`必须在选择器后面
2. 结尾的`}`在单独的一行
3. 每一个属性键值对要单独一行以分号结尾。
4. 单行注释

**app.less**

```
// app.less
@import "./global.less";

QWidget[classes~="bg-primary"],
QWidget[classes~="bg-secondary"] {
    &:hover{
        background-color: green;
    }
    QLabel{
        color:red;
    }

    background: @primaryColor;
}

QWidget[classes~="bg-secondary"] {
    background: @secondaryColor;
}
```

**global.less**

```
@import "./variable.less"

* {
padding: 0px;
}
// tooltip
QToolTip {
    padding: 3px 5px;
    background: white;
    border-radius: 4px;
    font-family: Arial;
}
// -- table
QHeaderView::section {
    background-color: white;
    font-size: 14px;
    font-family: "Arial MT";
    color: #222222;
    border: none;
}
```

**variable.less**

```
@primaryColor: #ff0000;
@secondaryColor: #00ff00;
```

## Todo

- [ ] 构建ast方式解析。
- [ ] 支持mixin
