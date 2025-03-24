1. 实验环境

腾讯云 CVM：标准型 SA5 机型，规格为 2核4GiB、100GiB 增强型 SSD 云硬盘。

2. 实验题目说明

参与者 fork 该项目，执行以下步骤执行 workload，尝试查找并修复三个可能的性能问题，使其运行得更快。

```
git clone https://github.com/xingfeng2510/workshop.git
cd workshop
make

./workshop
```

3. 实验要求：请你将使用到的优化手段和性能提升情况整理成 pdf 文档，和代码修改一并提交为 PR（pull request），且 PR 标题中带上运行提速倍数。

4.参与者最终性能提速倍数排行榜（后台周期更新）
<!-- RANKING_START -->
| 排名 | 提速倍数 | 作者 | 分支链接 |
|------|----------|------|--------|
| 1 |  4.64 | JaggerGu | [PR #1](https://github.com/xingfeng2510/workshop/pull/1) |
| 2 |  1.86 | X-Augenstern | [PR #3](https://github.com/xingfeng2510/workshop/pull/3) |
<!-- RANKING_END -->
