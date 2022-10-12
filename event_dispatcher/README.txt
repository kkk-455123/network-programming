事件分发机制
可以基于epoll/poll/select来实现

想法：
统一设计一个抽象的event_dispatcher类，此类中包含epoll等类，可以针对不同的需求来替换。是一种包含关系
