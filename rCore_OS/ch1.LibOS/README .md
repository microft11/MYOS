构建一个内核最小执行环境使得它能在 RV64GC 裸机上运行

移除它对于 Rust std标准库的依赖，因为 Rust std标准库自己就需要操作系统内核的支持

我们将其称为“三叶虫”操作系统