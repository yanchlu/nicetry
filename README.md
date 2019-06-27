
# 安装部署说明
[Android APP部署文档](#1)

[服务器部署文档](#2)

<h2 id='1'> Android APP部署文档 </h2>

<h2 id='2'> 服务器部署文档 </h2>

注：“赚闲钱”项目的服务器部署已经完成，不需要再次操作。若想尝试自己部署，可以参考以下步骤。

### 环境需求
- JAVA 8
- Apache Tomcat 9.0
- MySQL 8.0
### 安装步骤
1. 下载源代码
```shell
git clone https://github.com/sysu-abi/Server.git
```
根目录下有封装好的ServerAndDB.war文件

2. 下载Apache Tomcat9.0
参考链接（https://blog.csdn.net/bigpenga/article/details/81263007）

3. 部署项目
解压后，将根目录下的ServerAndDB.war文件放在Apache Tomcat 9.0服务器的根目录的webapps文件夹下即可。

数据库的当前位置是在服务器上。若想修改数据库地址，则可使用Eclipse导入ServerAndDB.war或者加载ServerAndDB文件夹下的JAVA项目（推荐使用前者）。
然后修改src文件夹下的mySpring.xml，将url、username和password设置为符合自己数据库的值。

4.使用postman测试相关接口

### 数据库建表
再次申明：所需的表已在服务器建立，若想尝试自己部署，可以自行参考。
1.创建用户表
```User
CREATE TABLE `User` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(30) NOT NULL,
  `phone` varchar(30) NOT NULL,
  `email` varchar(30) NOT NULL,
  `password` varchar(30) NOT NULL,
  `money` int(11) DEFAULT '10',
  `credit` int(11) DEFAULT '100',
  PRIMARY KEY (`uid`)
) ；
```
2.创建任务表
```Task
CREATE TABLE `Task` (
  `tid` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) NOT NULL,
  `title` varchar(30) NOT NULL,
  `detail` varchar(60) NOT NULL,
  `money` int(11) NOT NULL,
  `type` varchar(10) NOT NULL,
  `total_num` int(11) NOT NULL,
  `current_num` int(11) DEFAULT '0',
  `start_time` datetime DEFAULT NULL,
  `end_time` datetime DEFAULT NULL,
  `state` varchar(10) NOT NULL,
  PRIMARY KEY (`tid`),
  KEY `uid1` (`uid`),
  CONSTRAINT `uid1` FOREIGN KEY (`uid`) REFERENCES `User` (`uid`)
) ;
```
3.创建消息表
```Message
CREATE TABLE `Message` (
  `tid` int(11) NOT NULL,
  `uid` int(11) NOT NULL,
  `time` datetime DEFAULT NULL,
  `floor` int(11) NOT NULL,
  `detail` varchar(60) NOT NULL,
  KEY `tid3` (`tid`),
  KEY `uid3` (`uid`),
  CONSTRAINT `tid3` FOREIGN KEY (`tid`) REFERENCES `Task` (`tid`),
  CONSTRAINT `uid3` FOREIGN KEY (`uid`) REFERENCES `User` (`uid`)
) ;
```
4.创建UserJoins表
```UserJoins
CREATE TABLE `UserJoins` (
  `tid` int(11) NOT NULL,
  `uid` int(11) NOT NULL,
  `time` datetime NOT NULL,
  KEY `uid2` (`uid`),
  KEY `tid2` (`tid`),
  CONSTRAINT `tid2` FOREIGN KEY (`tid`) REFERENCES `Task` (`tid`),
  CONSTRAINT `uid2` FOREIGN KEY (`uid`) REFERENCES `User` (`uid`)
) ;
```
5.创建调查问卷表
```Survey
CREATE TABLE `Survey` (
  `tid` int(11) NOT NULL,
  `sid` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`sid`),
  KEY `tid_of_Survey` (`tid`),
  CONSTRAINT `tid_of_Survey` FOREIGN KEY (`tid`) REFERENCES `Task` (`tid`)
) 
```
6.创建问题表
```Question
CREATE TABLE `Question` (
  `sid` int(11) NOT NULL,
  `qid` int(11) NOT NULL,
  `qtype` varchar(30) NOT NULL,
  `qtitle` varchar(100) NOT NULL,
  `Answer_a` varchar(60) DEFAULT NULL,
  `Answer_b` varchar(60) DEFAULT NULL,
  `Answer_c` varchar(60) DEFAULT NULL,
  `Answer_d` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`sid`,`qid`),
  CONSTRAINT `sid_of_Question` FOREIGN KEY (`sid`) REFERENCES `Survey` (`sid`)
) 
```
7.创建答案统计表
```AnswerStatistics
CREATE TABLE `AnswerStatistics` (
  `sid` int(11) NOT NULL,
  `qid` int(11) NOT NULL,
  `count_a` int(11) DEFAULT NULL,
  `count_b` int(11) DEFAULT NULL,
  `count_c` int(11) DEFAULT NULL,
  `count_d` int(11) DEFAULT NULL,
  PRIMARY KEY (`sid`,`qid`),
  CONSTRAINT `key_of_AnswerStatistics` FOREIGN KEY (`sid`, `qid`) REFERENCES `Question` (`sid`, `qid`)
) 
```
8.创建事务表
```Session
CREATE TABLE `Session` (
  `uid` int(11) NOT NULL,
  `cookie` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`cookie`),
  KEY `cookie_of_Session` (`cookie`),
  KEY `uid_of_Session` (`uid`),
  CONSTRAINT `uid_of_Session` FOREIGN KEY (`uid`) REFERENCES `User` (`uid`)
) 
```

