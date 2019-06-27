
# 安装部署说明
[Android APP部署文档](#1)

[服务器部署文档](#2)

[数据库部署文档](#3)


<h2 id='1'> Android APP部署文档 </h2>

- 1.下载源码

  `git clone https://github.com/ssad2019/EE_easyeat_app`
  
- 2.使用Android Studio打开，导出生成apk
  - 在顶部选项中选择`Build -> Build APK(s)`
  - 生成的apk在`\app\build\outputs\apk\debug`中


<h2 id='2'> 服务器部署文档 </h2>

### 环境需求
- JAVA 8
- Tomcat9.0

### 安装步骤
1. 下载源代码
```shell
git clone https://github.com/sysu-abi/Server.git
```
根目录下有封装好的ServerAndDB.war文件

若想使用其他数据库，则

2.


8. 配置vhost文件
```shell
mkdir vhost
cd vhost
vim example.com.conf #这里将文件名改为要部署到的域名
```

填入如下内容并保存

```nginx
server {
  listen 80;

  # 如需开启HTTPS访问，请自行生成证书并取消如下代码的注释
  #listen 443 ssl http2;
  #ssl_certificate /usr/local/nginx/conf/ssl/xxx.crt;
  #ssl_certificate_key /usr/local/nginx/conf/ssl/xxx.key;
  #ssl_protocols TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;
  #ssl_ciphers TLS13-AES-256-GCM-SHA384:TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES-128-GCM-SHA256:TLS13-AES-128-CCM-8-SHA256:TLS13-AES-128-CCM-SHA256:EECDH+CHACHA20:EECDH+AES128:RSA+AES128:EECDH+AES256:RSA+AES256:EECDH+3DES:RSA+3DES:!MD5;
  #ssl_prefer_server_ciphers on;
  #ssl_session_timeout 10m;
  #ssl_session_cache builtin:1000 shared:SSL:10m;
  #ssl_buffer_size 1400;
  #add_header Strict-Transport-Security max-age=15768000;
  #ssl_stapling on;
  #ssl_stapling_verify on;

  server_name example.com; # 这里填入要绑定的域名
  access_log /data/wwwlogs/example.com_nginx.log combined;
  index index.html index.htm;
  root /data/wwwroot/web-client; # 这里填入刚刚复制到的网站目录

  # 如需将HTTP请求强行跳转为HTTPS，则取消如下代码注释
  #if ($ssl_protocol = "") { return 301 https://$host$request_uri; }

  #error_page 404 /404.html;
  #error_page 502 /502.html;

  location / {
    try_files $uri $uri/ /index.html?$args;
  }

  location ~ .*\.(gif|jpg|jpeg|png|bmp|swf|flv|mp4|ico)$ {
    expires 30d;
    access_log off;
  }

  location ~ .*\.(js|css)?$ {
    expires 7d;
    access_log off;
  }

  location ~ /\.ht {
    deny all;
  }
}
```

9. 启动Nginx
```shell
service nginx start
```

10. 使用浏览器访问域名测试

<h2 id='3'> Web服务端部署文档 </h2>

### 环境需求
- CentOS 7.0+
- PHP v7.0+ (`php-fpm` and `fastcgi` includes)
- Nginx v1.14.0+
- MySQL v5.4.0~v8.1.0
- Composer v1.6+
- Git
- Vim

### 部署步骤
1. 下载源代码并安装
```shell
mkdir /data/wwwroot/
cd /data/wwwroot/
git clone https://github.com/ssad2019/Web-Server-Side
chmod -R 777 Web-Server-Side
```

2. 配置[Config.php](https://github.com/ssad2019/Web-Server-Side/blob/master/settings/settings_sample.php)
```shell
cd Web-Server-Side/settings/
cp settings_sample.php settings.php
vim settings.php #修改说明见文件内容
```

3. 使用Composer安装依赖包
```shell
cd ..
composer install
```

4. 配置nginx.conf(前端部分已配置可跳过)
```shell
cd /usr/local/nginx
vim nginx.conf

# 在文件中此处末尾添加如下一行
http {
    ... # 这里包含其它配置
    include vhost/*.conf;
}
```

5. 配置FASTCGI文件
```
vim fastcgi.conf
```

填入如下内容并保存：
```nginx
fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
fastcgi_param  QUERY_STRING       $query_string;
fastcgi_param  REQUEST_METHOD     $request_method;
fastcgi_param  CONTENT_TYPE       $content_type;
fastcgi_param  CONTENT_LENGTH     $content_length;

fastcgi_param  SCRIPT_NAME        $fastcgi_script_name;
fastcgi_param  REQUEST_URI        $request_uri;
fastcgi_param  DOCUMENT_URI       $document_uri;
fastcgi_param  DOCUMENT_ROOT      $document_root;
fastcgi_param  SERVER_PROTOCOL    $server_protocol;
fastcgi_param  REQUEST_SCHEME     $scheme;
fastcgi_param  HTTPS              $https if_not_empty;

fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
fastcgi_param  SERVER_SOFTWARE    nginx/$nginx_version;

fastcgi_param  REMOTE_ADDR        $remote_addr;
fastcgi_param  REMOTE_PORT        $remote_port;
fastcgi_param  SERVER_ADDR        $server_addr;
fastcgi_param  SERVER_PORT        $server_port;
fastcgi_param  SERVER_NAME        $server_name;

# PHP only, required if PHP was built with --enable-force-cgi-redirect
fastcgi_param  REDIRECT_STATUS    200;
```

6. 配置vhost文件
```shell
mkdir vhost
cd vhost
vim api.example.com.conf #这里将文件名改为要部署到的域名
```

填入如下内容并保存
```nginx
server {
  listen 80;

  # 如需开启HTTPS访问，请自行生成证书并取消如下代码的注释
  #listen 443 ssl http2;
  #ssl_certificate /usr/local/nginx/conf/ssl/api.example.com.crt;
  #ssl_certificate_key /usr/local/nginx/conf/ssl/api.example.com.key;
  #ssl_protocols TLSv1 TLSv1.1 TLSv1.2 TLSv1.3;
  #ssl_ciphers TLS13-AES-256-GCM-SHA384:TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES-128-GCM-SHA256:TLS13-AES-128-CCM-8-SHA256:TLS13-AES-128-CCM-SHA256:EECDH+CHACHA20:EECDH+AES128:RSA+AES128:EECDH+AES256:RSA+AES256:EECDH+3DES:RSA+3DES:!MD5;
  #ssl_prefer_server_ciphers on;
  #ssl_session_timeout 10m;
  #ssl_session_cache builtin:1000 shared:SSL:10m;
  #ssl_buffer_size 1400;
  #add_header Strict-Transport-Security max-age=15768000;
  #ssl_stapling on;
  #ssl_stapling_verify on;

  server_name api.example.com; # 这里填入要绑定的域名
  access_log /data/wwwlogs/api.example.com_nginx.log combined;
  index index.html index.htm index.php;
  root /data/wwwroot/Web-Server-Side/app;

  # 如需将HTTP请求强行跳转为HTTPS，则取消如下代码注释
  #if ($ssl_protocol = "") { return 301 https://$host$request_uri; }

  #error_page 404 /404.html;
  #error_page 502 /502.html;
  
  location ~ [^/]\.php(/|$) {
    fastcgi_pass unix:/dev/shm/php-cgi.sock;
    fastcgi_index index.php;
    include fastcgi.conf;
  }

  location ~ .*\.(gif|jpg|jpeg|png|bmp|swf|flv|mp4|ico)$ {
    expires 30d;
    access_log off;
  }

  location ~ .*\.(js|css)?$ {
    expires 7d;
    access_log off;
  }

  location ~ /\.ht {
    deny all;
  }
}
```

7. 启动Nginx
```shell
service nginx start
```

8. 通过浏览器或Postman等进行API测试
