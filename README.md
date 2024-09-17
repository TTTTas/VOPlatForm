# VOPlatForm

## TODO Lists

- [ ] 添加三种任务的解决方案**静态类**
- [ ] 根据三种任务的解决方案类编写项目管理文件的文件结构
- [ ] 项目管理器中的文件结构设计
- [ ] Log输出台的输入接口编写
- [ ] Log输出台的输出接口编写
- [ ] 相机模型管理器编写
- [ ] temp文件管理架构编写
- [ ] VO过程梳理
- [ ] 视频解码器和窗口展示器编写（目前还不能展示视频）

## 需要注意的点

- [ ] 所有的解决方法要抽象成静态类，方便调用和管理
- [ ] 相同的方法最好提取到同一个类，或直接变成内联函数，方便调用
- [ ] 涉及到相机模型的部分使用我已经编写了的**camera.h**中的类进行管理，需要的话适当对其修改即可
- [ ] 一定要保留好接口设计
- [ ] 所有文件命名要规范，不要随便命名
- [ ] **加注释加注释加注释**
- [ ] 啥时候把后端具体分工做一下，方便项目推进

## 工程项目结构

``` markdown
Project
├── File Header
│   ├── File Name
│   ├── File Type
│   └── File Date(尚未添加进去)
├── File Data
│   ├── Project Manager
│   │   ├── Project File(folder)
│   │   ├── Project Data
│   │   └── Project Log
│   ├── Camera Model
│   │   ├── Camera Model File
│   │   ├── Camera Model Data
│   │   └── Camera Model Log
│   ├── Log Output
│   │   ├── Log File
│   │   ├── Log Data

