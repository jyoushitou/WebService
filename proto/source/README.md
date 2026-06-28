# RPC Proto 仓库

## 目录结构

```
proto/
├── source/          # Proto源文件
│   ├── common/      # 公共类型定义
│   ├── gateway/     # 网关服务接口
│   ├── service/     # 业务服务接口
│   ├── mysql/       # 数据库服务接口
│   ├── cert/        # 证书服务接口
│   ├── frontend/    # 前端服务接口
│   └── CMakeLists.txt
├── build/           # 编译输出目录
│   ├── lib/         # 编译后的库文件
│   ├── bin/         # 可执行文件
│   └── generated/   # 生成的代码
└── README.md
```

## 编译方法

```bash
cd proto/source
mkdir -p ../build
cd ../build
cmake ../source
make -j$(nproc)
```

## 在其他仓库中使用

### CMake方式引用

```cmake
# 在微服务的CMakeLists.txt中
find_package(rpc_protos REQUIRED)
target_link_libraries(your_service rpc_protos)
```

### 手动引用

```bash
# 将编译后的库和头文件复制到微服务项目中
cp -r proto/build/lib/* your_service/lib/
cp -r proto/build/generated/* your_service/include/
```

## 服务列表

| 服务 | 文件 | 说明 |
|------|------|------|
| common | common.proto, error_code.proto | 公共类型和错误码 |
| gateway | gateway_service.proto | 网关统一入口 |
| service | business_service.proto | 核心业务逻辑 |
| mysql | db_service.proto | 数据库操作 |
| cert | cert_service.proto | SSL证书管理 |
| frontend | frontend_service.proto | Vue前端接口 |