封装了Qt http的操作

# Qt5.12.3+ http的坑
运行后遇到一个报错
```
qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed
```
这是一个很常见的错误，缺少openssl的动态库libeay32.dll和ssleay32.dll导致的

熟练的找到这两个库，复制到exe同级目录下，what？还是报错？确认libeay32.dll和目标exe 位数一样（32位），
查阅资料可知，Qt5.12.3以后，http相关实现使用的openssl改成了1.1.1版本，openssl 1.1.1版本相比openssl 1.0.2版本改动较大，
连动态库名称都变了，改为了libssl-1_1.dll和libcrypto-1_1.dll（dll右键-属性-详细信息可以看到版本号为1.1.1），[在这里](http://slproweb.com/products/Win32OpenSSL.html)下载对应位数的openssl动态库，
复制到exe同级目录下解决。