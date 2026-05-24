# Git 协作流程

本文档用于说明本项目多人合作时 Git 和 GitHub 的使用方式。

## 1. 基本原则

不要多人直接在 `main` 分支上同时改代码。

推荐流程：

```text
main 分支：只放稳定代码
feature 分支：每个人开发自己的功能
Pull Request：合并前审查代码
```

简单说：

```text
先从 main 拉最新代码
再开自己的分支开发
开发完成后 push 到 GitHub
最后通过 Pull Request 合并到 main
```

## 2. 第一次下载工程

队友第一次拿代码时，执行：

```bash
git clone https://github.com/PatienceWorks/Ximenzi.git
```

进入工程目录：

```bash
cd Ximenzi
```

然后用 Keil 打开：

```text
01 CIMC_GD32_Template/project/CIMC_GD32_Template.uvprojx
```

## 3. 每天开始前先同步 main

开始写代码前，先切回 `main`：

```bash
git checkout main
```

拉取 GitHub 最新代码：

```bash
git pull
```

确认本地状态干净：

```bash
git status
```

如果看到：

```text
nothing to commit, working tree clean
```

说明本地没有未提交修改，可以开始新功能。

## 4. 新建自己的功能分支

每个人开发一个功能时，新建自己的分支。

例如开发 ADC：

```bash
git checkout -b feature/adc
```

例如开发串口命令：

```bash
git checkout -b feature/command
```

例如开发 RTC：

```bash
git checkout -b feature/rtc
```

推荐分支命名：

```text
feature/adc
feature/command
feature/rtc
feature/oled
feature/storage
feature/sample
fix/usart-bug
```

查看当前在哪个分支：

```bash
git branch
```

带 `*` 的就是当前分支。

## 5. 修改代码

切到自己的分支后，就可以正常用 Keil、VSCode 或记事本修改本地代码。

注意：切换分支时，本地文件内容会跟着变化。

所以修改代码前一定要确认当前分支：

```bash
git branch
```

不要在错误的分支上改代码。

## 6. 查看自己改了什么

查看哪些文件被修改：

```bash
git status
```

查看具体代码改动：

```bash
git diff
```

如果已经执行过 `git add .`，查看暂存区改动：

```bash
git diff --cached
```

红色表示删除的旧代码，绿色表示新增的新代码。

## 7. 提交代码

确认代码没问题后，添加改动：

```bash
git add .
```

提交：

```bash
git commit -m "Add ADC sampling"
```

提交信息要写清楚做了什么，例如：

```text
Add USART command parser
Add RTC time config
Fix ADC voltage calculation
Add OLED sample display
Remove temporary debug code
```

## 8. 上传自己的分支

第一次上传当前分支：

```bash
git push origin feature/adc
```

把 `feature/adc` 换成自己的分支名。

例如：

```bash
git push origin feature/command
```

如果这个分支之前已经上传过，后面也可以继续用：

```bash
git push
```

## 9. 创建 Pull Request

分支 push 到 GitHub 后，打开仓库页面：

```text
https://github.com/PatienceWorks/Ximenzi
```

一般 GitHub 会提示：

```text
Compare & pull request
```

点击后创建 Pull Request。

目标分支选择：

```text
base: main
compare: feature/自己的分支
```

Pull Request 标题建议写清楚：

```text
Add ADC sampling
Add command parser
Add RTC config command
```

描述里写：

```text
完成了什么
怎么测试的
还有什么问题
```

## 10. 审查别人代码

审查 Pull Request 时，重点看：

```text
有没有改错别人的模块
有没有把题目逻辑写进驱动文件
有没有忘记添加 .c 文件
有没有忘记添加头文件路径
有没有临时测试代码
Keil 是否能编译通过
功能是否已经上板测试
```

在 GitHub 的 Pull Request 页面点击：

```text
Files changed
```

可以看到具体代码改动。

红色表示删除，绿色表示新增。

审查通过后，再合并到 `main`。

## 11. 合并后其他人怎么更新

某个 Pull Request 合并进 `main` 后，其他人需要同步最新代码：

```bash
git checkout main
git pull
```

如果自己还在功能分支上开发，可以把最新的 `main` 合到自己的分支：

```bash
git checkout feature/自己的分支
git merge main
```

如果出现冲突，先不要乱改，找队友一起看冲突文件。

## 12. 切换到别人的分支

先更新远程分支信息：

```bash
git fetch
```

查看远程分支：

```bash
git branch -r
```

如果看到：

```text
origin/feature/adc
```

可以切换到本地：

```bash
git checkout -b feature/adc origin/feature/adc
```

如果本地已经有这个分支，直接：

```bash
git checkout feature/adc
```

切换分支前，先确认没有未提交修改：

```bash
git status
```

## 13. 常见问题

### 13.1 push 被拒绝

如果看到类似：

```text
rejected
fetch first
```

说明 GitHub 上有你本地没有的更新。

先执行：

```bash
git pull
```

然后再：

```bash
git push
```

### 13.2 进入 vim 编辑器

如果执行 `git pull` 时进入一个黑色编辑器界面，不要慌。

保存退出：

```text
Esc
:wq
Enter
```

如果想避免进入编辑器，可以用：

```bash
git pull --no-edit
```

### 13.3 中文文件名乱码

有时终端会把中文文件名显示成乱码，但文件本身不一定有问题。

为了减少问题，建议：

```text
代码文件名尽量用英文
目录名尽量用英文
文档可以用中文
```

## 14. 每天推荐流程

每天开始：

```bash
git checkout main
git pull
git checkout -b feature/今天要做的功能
```

开发过程中：

```bash
git status
git diff
```

开发完成：

```bash
git add .
git commit -m "说明这次改了什么"
git push origin feature/自己的分支
```

然后去 GitHub 创建 Pull Request。

## 15. 本项目建议分支

可以按模块分支开发：

```text
feature/command      串口命令解析
feature/sample       采样启停和周期控制
feature/adc          ADC 电压采集
feature/rtc          RTC 时间设置和读取
feature/oled         OLED 显示
feature/storage      TF 卡和 Flash 存储
feature/hide-unhide  hide/unhide 数据处理
```

## 16. 最重要的习惯

每次改代码前：

```bash
git branch
git status
```

每次提交前：

```bash
git diff
```

每次开始新功能前：

```bash
git checkout main
git pull
git checkout -b feature/功能名
```

不要直接把未测试代码合进 `main`。

`main` 应该始终保持能编译、能运行。
