# bootload配置
1. IAP_FLAG_ADDR 读取IAP标志位地址 uint64_t 由高字节0x1225 低字节是包数量 0x807f800 最后2k
2. APPLICATION_ADDRESS  跳转APP地址 0x8008000  32k
