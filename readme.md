php-inotify用于监控目录或文件事件

### 编译安装
```bash
/path/to/phpize
./configure --with-php-config=/path/to/php-config
make && make install
```

### php脚本
```php
// inotify.php
$inotify = new Inotify();
$inotify->addWatch(['/home/wuzhc']); // 添加监控目录或文件

while (true) {
  $ret = $inotify->run(); 
  print_r($ret); // [{name:"/home/wuzhc/test.txt", mask:IN_MODIFY}]
}
```

### 运行
```bash
/path/to/php inotify.php
```

### 说明
```php
$inotify->addWatch(array path);
```
- path 监控目录或文件，类型为数组，可以同时监控多个文件或目录

```php
$inotify->run();
```
run运行时会阻塞程序，直到有事件发生会返回数据，然后继续下次监控