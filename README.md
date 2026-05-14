# sjtu_contender

## 一个交我办第二课堂自动报名软件

---

使用到的NuGet程序包：**Microsoft.Web.WebView2**, **Microsoft.Windows.ImplementationLibrary**

---

**WebView2**层级关系: Environment -> Controller -> WebView
- **ICoreWebView2Environment(环境/工厂)**: 这是第一步，通过*CreateCoreWebView2EnvironmentWithOptions*函数来获取这个接口的**实例(environment)**
- **ICoreWebView2Controller(控制器)**: 这是第二步，使用上一步得到的*environment*来调用*CreateCoreWebView2Controller*，从而得到这个**控制器(controller)**。它像是浏览器的“外壳”。
- **ICoreWebView2(核心视图)**: 这是第三步，从*controller*中获取*CoreWebView2*。它代表了浏览器的“内核”和功能逻辑，比如用它来执行**JavaScript**的*ExecuteScript*方法
