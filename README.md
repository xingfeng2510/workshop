1.测试环境（自行按要求购买）

在腾讯云官网购买cvm(https://cloud.tencent.com/product/cvm)，
购买南京地域的标准型SA5机型，规格为2核4GiB内存+100GiB增强型SSD云硬盘（注意：压测此数据盘，不要压测系统盘）
费用参考：按量计费 0.6/小时
![企业微信截图_da8e1b99-29fb-4b48-8cf1-94b68d98b4f8](https://github.com/user-attachments/assets/10e5a8b8-748e-4d17-b393-10d290dd17ea)
![image](https://github.com/user-attachments/assets/f74017b6-9e1c-4638-b9c0-69f3708cd07a)

2.测试题目说明

参与者 fork 该项目，执行以下步骤执行 workload，尝试查找并修复三个可能的性能问题，使其运行得更快。

```
git clone https://github.com/xingfeng2510/workshop.git
cd workshop
make

./workshop
```

3.测试要求：请你将使用到的优化手段和性能提升情况整理成 pdf 文档，和代码修改一并提交为 PR（pull request），且 PR 标题中带上运行提速倍数。

4.参与者最终性能提速倍数排行榜（后台周期更新）
<!-- RANKING_START -->
| 排名 | 提速倍数 | 作者 | 分支链接 |
|------|----------|------|--------|
| 1 |  4.64 | JaggerGu | [PR #1](https://github.com/xingfeng2510/workshop/pull/1) |
| 2 |  1.86 | X-Augenstern | [PR #3](https://github.com/xingfeng2510/workshop/pull/3) |
<!-- RANKING_END -->
