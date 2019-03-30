# 锚定币合约设计方案  

# 第一部分：简介

锚定币合约为系统合约，在主网启动时部署在 BOS 链上，用于在 BOS 链上提供 USDT/BTC/ETH 与其主网的自由等比兑换。

为表述方便，定义一些名词：

```
角色：合约部署者（deployer）、承兑商(issuer)、承兑商相应币种承兑人（acceptor）、承兑商审核员（auditor）、普通用户(user)

合约部署者在主网启动时部署该合约。
USDT部署者是usdt.bos;
BTC部署者是btc.bos;
ETH部署者是eth.bos;




承兑商由BP多签创建，每种币可以有多个承兑商。但在BOS链上的币名不能相同。
举例：BTC在BOS链上的相应的币名可以为BTC、BBTC、BOSBTC等。
一个承兑商可以承兑多种币，一种币只能用一个承兑商。

每个承兑商相应币种的承兑人有且只有一个。
承兑人的功能：（a）给用户转账（b）接收用户转账

每个承兑商的审核员至少一个、可以多个。
审核员的功能：（a）同意或拒绝大额提币 （b）提币失败时, 将币退回用户账户。

充币：在BOS链上，承兑商的承兑人给普通用户转账

提币：在BOS链上，普通用户给承兑商的承兑人转账

合约部署账号:  usdt.bos btc.bos  eth.bos
承兑商(issuer)账号:   boshuobipool
承兑商相应币种承兑人（acceptor）的账号:
	BTC:  btcbtcbtcbtc
	ETH:  ethethetheth
    USDT: usdtusdtusdt

承兑商审核员（auditor）账号： huobiauditor, auditorhuobi
承兑商合作方(partner)账号:  huobipartner, partnerhuobi
```

# 第二部分：接口设计

# 一、 create【BP 多签】

功能： 创建承兑商及币种。
承兑商账号不能是合约部署账号，如 btc.bos。
权限:   BP 多签

```
参数：  
symbol  sym  //币名。如BTC、ETH、USDT   包含精度
name    issuer     //承兑商账户, 如boshuobipool
name    acceptor     //承兑商的承兑人账户
name address_style  //地址类型: bitcoin、ethereum、tether
string   organization   //承兑商简介
string  website        //承兑商官网地址
```

大致流程：
创建币名、承兑商、承兑人、地址类型、简介、网址，填充 symbols 表的 sym 字段和 stat 表的 issuer、acceptor、address_style、organization、website 字段。

同时初始化：
supply：BTC   0.00000000        ETH    0.00000000  USDT    0.00000000
maximum_limit：BTC   0       ETH    0   USDT    0
minimum_limit：BTC   0    ETH     0   USDT    0
total_limit:   BTC  0     ETH   0  USDT    0
frequency_limit:  0
interval_limit:  0
delay: 0
fee_rate: 0
min_fee:  BTC 0   ETH 0  USDT    0
active: true

注意相应币种的精度：BTC  8    ETH 8  USDT    8

```
cleos push action btc.bos create '["8,BTC", "boshuobipool", "btcbtcbtcbtc", "bitcoin",  "huobipool", "http://www.huobipool.com"]' -p btc.bos
cleos push action eth.bos create '["8,ETH", "boshuobipool", "ethethetheth", "ethereum", "huobipool", "http://www.huobipool.com"]' -p eth.bos
cleos push action usdt.bos create '["8,USDT", "boshuobipool", "usdtusdtusdt", "tether", "huobipool", "http://www.huobipool.com"]' -p usdt.bos
```

# 二、 update

```
功能：承兑商更新自己信息
权限：承兑商账号, 如boshuobipool
参数：  
symbol_code  sym_code  //币名。如BTC、ETH、USDT
string organization  //organization
string website      //website
```

大致流程：
校验权限。
根据 sym_code，修改 stat 表的 organization、website 字段

```
cleos push action btc.bos update '["BTC", "火币矿池", "http://www.huobipool.com"]' -p boshuobipool
cleos push action eth.bos update
'["ETH","火币矿池", "http://www.huobipool.com"]' -p boshuobipool
cleos push action usdt.bos update '["USDT","火币矿池", "http://www.huobipool.com"]' -p boshuobipool
```

# 三、 setlimit

功能：承兑商设置单次最小换币金额、单次最大换币金额、单日累计换币次数、单日累计换币限额
权限：承兑商账号, 如 boshuobipool
参数：

```
asset   maximum_limit  //单次最大换币金额。每个账号每次换币金额大于此数, 承兑商不换币。默认值为：BTC 1   ETH 1  USDT 1
asset   minimum_limit  //单次最小换币金额。每个账号每次换币金额小于此数，承兑商不换币。默认值为：BTC 0.00005  ETH 0.00005 USDT 0.00005
asset   total_limit  //单日累计换币限额。每个账号每日换币总额大于此数，承兑商不换币。默认值为：BTC 10 ETH 10  USDT 10
uint64_t frequency_limit   //单日累计换币次数。 每个账号每日换币次数大于此数，则承兑商不换币。默认值为：BTC 3 ETH 3  USDT 3
uint64_t interval_limit //两次换币的间隔秒数。每个账号两次换币间隔秒数小于次数，承兑商不换币。默认值为：300
```

大致流程：
校验权限。
根据 asset 的 symbol，修改 stats 表的 maximum_limit、minimum_limit、total_limit、frequency_limit、interval_limit 字段
需要判断所有的 asset 是否为同一种币。

```
cleos push action btc.bos setlimit '["1.00000000 BTC","0.00005000 BTC","10.00000000 BTC","3","300"]' -p boshuobipool
cleos push action eth.bos setlimit
'["1.00000000 ETH","0.00005000 ETH","10.00000000 ETH","3","300"]' -p boshuobipool
cleos push action usdt.bos setlimit '["1.00000000 USDT","0.00005000 USDT","10.00000000 USDT","3","300"]' -p boshuobipool
```

# 四、 setsinglelimit

功能：承兑商设置单个用户的单次最小换币金额、单次最大换币金额、单日累计换币次数、单日累计换币限额
权限：承兑商账号, 如 boshuobipool
参数：

```
name  vip //VIP用户账号
asset   maximum_limit  //单次最大换币金额。每个账号每次换币金额大于此数, 承兑商不换币。默认值为：BTC 1   ETH 1  USDT 1
asset   minimum_limit  //单次最小换币金额。每个账号每次换币金额小于此数，承兑商不换币。默认值为：BTC 0.00005  ETH 0.00005 USDT 0.00005
asset   total_limit  //单日累计换币限额。每个账号每日换币总额大于此数，承兑商不换币。默认值为：BTC 10 ETH 10  USDT 10
uint64_t frequency_limit   //单日累计换币次数。 每个账号每日换币次数大于此数，则承兑商不换币。默认值为：BTC 3 ETH 3  USDT 3
uint64_t interval_limit //两次换币的间隔秒数。每个账号两次换币间隔秒数小于次数，承兑商不换币。默认值为：300
```

大致流程：
校验权限。
根据 asset 的 symbol 和 vip 的账户名，修改 vips 表的 maximum_limit、minimum_limit、total_limit、frequency_limit、interval_limit 字段
需要判断所有的 asset 是否为同一种币。

```
cleos push action btc.bos setsinglelimit '["huobihuaping", "1.00000000 BTC","0.00005000 BTC","10.00000000 BTC","3","300"]' -p boshuobipool
cleos push action eth.bos setsinglelimit
'["huobihuaping", "1.00000000 ETH","0.00005000 ETH","10.00000000 ETH","3","300"]' -p boshuobipool
cleos push action usdt.bos setsinglelimit '["huobihuaping", "1.00000000 USDT","0.00005000 USDT","10.00000000 USDT","3","300"]' -p boshuobipool
```

# 五、 setfee

功能：承兑商设置服务费。
提币收取，充币不收取。
按提币金额收取相应费率的手续费。
有最低收费，因为币价有波动，所以设置 min_fee。
权限：承兑商账号, 如 boshuobipool
参数：

```
double  service_fee_rate     //服务费率   默认0.001 （千分之一）
asset   min_service_fee     //服务费最低收费  默认0.0005
asset   miner_fee  //矿工费  默认 BTC 0.00004 ETH 0.00004
```

大致流程：
校验权限。
根据 min_service_fee 和 miner_fee 的 symbol，修改 stats 表的 service_fee_rate、min_service_fee、miner_fee 字段。
记得判断 min_service_fee、miner_fee 是否为同一种币。

```
cleos push action btc.bos setfee '["0.001","0.00005000 BTC", "0.00004000 BTC"]' -p boshuobipool
cleos push action eth.bos setfee
'["0.001","0.00005000 ETH", "0.00004000 ETH"]' -p boshuobipool
cleos push action usdt.bos setfee '["0.001","0.00005000 USDT", "0.00004000 USDT"]' -p boshuobipool
```

# 六、 setsinglefee

功能：承兑商设置单个用户的服务费。
提币收取，充币不收取。
按提币金额收取相应费率的手续费。
有最低收费，因为币价有波动，所以设置 min_fee。
权限：承兑商账号, 如 boshuobipool
参数：

```
name vip //VIP用户账号
double  service_fee_rate     //服务费率   默认0.001 （千分之一）
asset   min_service_fee     //服务费最低收费  默认0.0005
asset   miner_fee  //矿工费  默认 BTC 0.00004 ETH 0.00004
```

大致流程：
校验权限。
根据 min_service_fee 和 miner_fee 的 symbol，以及 vip 的账户名, 修改 vips 表的 service_fee_rate、min_service_fee、miner_fee 字段。
记得判断 min_service_fee、miner_fee 是否为同一种币。

```
cleos push action btc.bos setsinglefee '["huobihuaping","0.001","0.00005000 BTC", "0.00004000 BTC"]' -p boshuobipool
cleos push action eth.bos setsinglefee
'["huobihuaping", "0.001","0.00005000 ETH", "0.00004000 ETH"]' -p boshuobipool
cleos push action usdt.bos setsinglefee '["huobihuaping", "0.001","0.00005000 USDT", "0.00004000 USDT"]' -p boshuobipool
```

# 七、setdelay

功能：  承兑商设置换币记录保存时间。
如不设置默认为 7 天。
必须大于等于 1。
权限：承兑商的账号，如 boshuobipool
参数：
symbol_code sym_code  //币名。如 BTC、ETH、USDT
uint64_t delayday      //天数

大致流程：
校验权限。
根据 sym_code 确定 scope。
修改 stat 的 delay 字段。

```
cleos push action btc.bos setdelay '["BTC","7"]' -p boshuobipool
cleos push action eth.bos setdelay '["ETH","8"]' -p boshuobipool
cleos push action usdt.bos setdelay '["USDT","8"]' -p boshuobipool
```

# 八、 issue

功能：承兑商发行币。
由承兑商保证不滥发，做到与承兑商在相应的币的主网锁定的币数量一致，保持 1：1 兑换。例如：承兑商在 BTC 主网的账户锁定 1000 个 BTC，则在 BOS 链也发行 1000 个 BTC。
权限：承兑商账号, 如 boshuobipool
参数：

```
asset quantity       //需要发行的相应币种的总额
string memo        //备注
```

大致流程：
校验权限。
quantity 必须大于 0 BTC 精度为 8 ETH 精度为 8 USDT 精度为 8
memo 可以为空
根据 quantity 所携带的 symbol 信息确定 scope
增加 acceptor 的余额
增加 stat 表中 supply 的值
向 operates 表插入一条数据, type 置为 1。

```
cleos push action btc.bos issue '["10000.00100000 BTC","发行BTC"]' -p boshuobipool
cleos push action eth.bos issue '["1990.00100091 ETH","发行ETH"]' -p boshuobipool
cleos push action usdt.bos issue '["1990.00100091 USDT","发行USDT"]' -p boshuobipool
```

# 九、 retire

功能：承兑商销毁币。
由承兑商保证不滥发，做到与承兑商在相应的币的主网锁定的币数量一致，保持 1：1 兑换。例如：承兑商在 BTC 主网的账户锁定的 BTC 由 1000 个变成 800 个，则在 BOS 链销毁 200 个 BTC。
权限：承兑商账号, 如 boshuobipool
参数：

```
asset quantity       //需要销毁的相应币种的总额
string memo        //备注
```

大致流程：
校验权限。
quantity 必须大于 0 BTC 精度为 8 ETH 精度为 8 USDT 精度为 8
memo 可以为空
根据 quantity 所携带的 symbol 信息确定 scope
减小 acceptor 的余额
减小 stat 表中 supply 的值
向 operates 表插入一条数据, type 置为 0。

```
cleos push action btc.bos retire '["100.00100000 BTC","销毁BTC"]' -p boshuobipool
cleos push action eth.bos retire '["190.00100091 ETH","销毁ETH"]' -p boshuobipool
cleos push action usdt.bos retire '["190.00100091 USDT","销毁USDT"]' -p boshuobipool
```

# 十、 applyaddr

功能：用户申请分配地址。
此账号必须在 BOS 上真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种承兑人的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。
权限：普通用户
参数：

```
symbol_code sym_code  //币名。如BTC、ETH、USDT
name to   //需要分配地址的账号
```

大致流程：
校验权限。
校验 to 是否为合法的 BOS 账户。
to 不能是上述的特殊账号。
根据 sym_code，在 addrs 表中增加一条记录。
此时 owner 字段置为 to，address 字段为空， state 字段为 owner.value，
create_time 为当前时间，assign_time 为空

```
cleos push action btc.bos applyaddr '[ "BTC", "xiaomingming" ]' -p xiaomingming
cleos push action eth.bos applyaddr '[ "ETH", "huobihuaping" ]' -p huobihuaping
cleos push action usdt.bos applyaddr '[ "USDT", "huobihuaping" ]' -p huobihuaping
```

# 十一、assignaddr 【承兑人】

功能：承兑商给账户分配地址。
此账号必须在 BOS 上真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种承兑人的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。 需要校验地址合法性。
权限：承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth
参数：

```
symbol_code sym_code  //币名。如BTC、ETH、USDT
name to   //需要分配地址的账号
string address  //相应币种地址，如BTC、ETH、USDT
```

大致流程：
校验权限。
校验 address 是否为相应币种的合法地址。
校验 to 是否为合法的 BOS 账户。
to 不能是上述的特殊账号。
根据 sym_code，在 addrs 表中修改一条记录。
查找 owner 等于 to 的记录，修改 address 字段， state 字段置为 0，assign_time 为当前时间
address 按照固定格式存：TYPE:ADDR-TYPE:ADDR
TYPE 的取值为 BTC 或 ETH

```
cleos push action btc.bos assignaddr '["BTC", "huobihuaping", "1F7XiwJTfZEpFDGikNC71U6MBBeLsrsDd6"]' -p btcbtcbtcbtc
cleos push action eth.bos assignaddr '["ETH", "huobihuaping", "0x8232Fe62877517895ee43FFFB0B079E69293d5e2"]' -p btcbtcbtcbtc
cleos push action usdt.bos assignaddr '["USDT", "huobihuaping", "1F7XiwJTfZEpFDGikNC71U6MBBeLsrsDd6"]' -p btcbtcbtcbtc
```

# 十二、withdraw

在 BOS 链用户给火币转账
功能：普通用户从 BOS 链向其他链换币。  
用户将资产转给承兑商相应币种承兑人。
普通用户账号必须真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种承兑人的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。

满足以下任一一点时，不允许提币也不写入表中：
(a) 申请提币金额小于单次最小额度
(b) 申请提币金额大于单次最大额度
(c) 提币申请距离上次提币时间的间隔秒数小于最小间隔秒数
(d) 从上一个自然日零点到申请提币时，该账户累计提币次数大于单日累计换币次数
(e) 从上一个自然日零点到申请提币时，该账户累计提币金额大于单日累计换币限额

权限：普通用户的账号
参数：

```
name from  //普通用户账号。
string to  //提币的目的地。若为BTC或ETH，则该字段填入BTC或ETH的地址。需要校验BTC地址、ETH地址的合法性。
asset quantity  //需要换币的资产总数
string memo  //备注
```

大致流程：
校验权限。
校验 from 是否为合法的 BOS 账号；
from 不能是上述的特殊账号。
校验 to 是否为合法的 BTC 地址、ETH 地址、USDT 地址。
quantity 必须大于 0。 注意精度。
根据 quantity 携带 synbol 确定 scope。
若符合“禁止提币”的条件，则返回。其中：若为 vip 用户，则判断该用户的限制条件；若不是 vip 用户，则使用通用的限制条件。
若为其他情况，则在 withdraws 添加一条记录，且将 enable 字段置为 true。
以 owner 为主键，查找 statistics 表，若不存在，则添加；若存在，则更新 statistics 表，last_time 字段为当前时间，frequency 加 1，total 加 quantity，update_time 为当前时间。
statistics 表的记录在每日零点清除。也可以在添加或修改记录时，判断 update_time 的日期，若为前一日的日期，则先删除再添加。
生成交易号填充 withdraws 的 trx_id 字段。
注：USDT 提币有 ERC20 和 BTC 两种，memo 需要填写 type，to 的地址校验根据 type 类型来做. type 取值为 BTC 或者 ETH

```
cleos push action btc.bos withdraw '[ "huobihuaping", "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB", "0.90000000 BTC", "BTC" ]' -p huobihuaping
cleos push action eth.bos withdraw '[ "huobihuaping", "0xe53d7a25f9b1769d0a9b6ae674155b6be886252d
", "0.90000000 ETH", "ETH" ]' -p huobihuaping
cleos push action usdt.bos withdraw '[ "huobihuaping", "0xe53d7a25f9b1769d0a9b6ae674155b6be886252d", "0.90000000 USDT", "ETH" ]' -p huobihuaping
cleos push action usdt.bos withdraw '[ "huobihuaping", "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB", "0.90000000 USDT", "BTC" ]' -p huobihuaping
```

# 十三、deposit

在 BOS 链上，huobi 给普通用户转账
功能：用户从其他链向 BOS 链换币。
承兑商相应币种承兑人给相应账户转账。
此账号必须真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。
调用 notify 插件接口通知该账号, 将来前端可以监听到并提示。
设置 defer，删除满足条件的记录。即满足任意一点：
(a）充币记录，距离 create_time 的间隔超过指定时间（比如 7 天）
权限：承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth
参数：

```
string  from  //BTC地址、ETH地址、USDT地址
name to  //普通用户账号。
asset quantity  //需要换币的资产总数
string               remote_trx_id  //其他链的交易号
string memo  //备注
```

大致流程：
校验权限。
校验 to 是否为合法的 BOS 账号；
to 不能是上述的特殊账号。
quantity 必须大于 0。 注意精度。
memo 可以为空。
根据 quantity 携带 synbol 确定 scope。
生成交易号填充 deposits 的 trx_id 字段。
create_time 为当前时间。
msg 填充 memo 字段。
向 deposits 表插入一条数据

```
cleos push action btc.bos deposit '[  "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB", "huobihuaping", "0.90000000 BTC", "fe2f90783080925ef63b9369f3e00df6860d921806655543f582e869e4652619"，"BTC充币" ]' -p btcbtcbtcbtc

cleos push action eth.bos deposit '[ "0xe53d7a25f9b1769d0a9b6ae674155b6be886252d", "huobihuaping",  "0.90000000 ETH", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "ETH充币" ]' -p ethethetheth

cleos push action usdt.bos deposit '[  "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB", "huobihuaping", "0.90000000 USDT", "fe2f90783080925ef63b9369f3e00df6860d921806655543f582e869e4652619"，"USDT充币" ]' -p usdtusdtusdt
```

# 十四、transfer

功能：普通用户互相转账。
账号必须真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。
权限：普通用户的账号
参数：

```
name    from  //转出账号。
name    to //转入账号。
asset   quantity  //需要转账的资产总数
string  memo  //备注
```

大致流程：
校验权限。
校验 to 是否为合法的 BOS 账号；
to 不能是上述的特殊账号。
校验 from 是否为合法的 BOS 账号；
from 不能是上述的特殊账号。
quantity 必须大于 0。 注意精度。
memo 可以为空。
根据 quantity 携带 symbol 确定 scope。

```
cleos push action btc.bos transfer '[ "huobihuaping", "huobiliqiang", "0.90000000 BTC", "BTC转账" ]' -p huobihuaping
cleos push action eth.bos transfer '[ "huobihuaping", "huobiliqiang
", "0.90000000 ETH", "ETH转账" ]' -p huobihuaping
cleos push action usdt.bos transfer '[ "huobihuaping", "huobiliqiang", "0.90000000 USDT", "USDT转账" ]' -p huobihuaping
```

# 十五、feedback【通知：notify_plugin】

功能：  从 BOS 链向其他链换币成功时，承兑商反馈信息。
此时 state 设为 2。
需要校验其他链的交易号的有效性。
调用 notify 插件接口通知该账号, 将来前端可以监听到并提示。
设置 defer，删除满足条件的记录。
即满足任意一点：
(a) 提币记录，距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 state=2（提币成功）或者 state=3（提币已退回）  
(b) 提币记录，距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 quantity 小于设定的单次最小提币金额

权限：承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth
参数：

```
symbol_code sym_code  //币名。如BTC、ETH
transaction_id_type trx_id  //BOS链上提币的交易号
string remote_trx_id  //其他链上提币的交易号
string memo  //备注
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
withdraws 表的 enable 必须为 true，并且 state 必须等于 0，才可执行。
根据 trx_id 查询 withdraws 表，填充 withdraws 表中的 remote_trx_id 字段。
memo 可以为空。
将 state 置为 2.
调用 notify 插件通知用户。可以根据 trx_id 查询 withdraws 表得到用户名。
设置 defer，定时删除符合条件的记录。

```
cleos push action btc.bos feedback '[ "BTC", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "faee959898c691c89fc7497f3ab5ab08c2cc7865f48064b803781efa8958cd65", "BTC反馈" ]' -p btcbtcbtcbtc

cleos push action eth.bos feedback '[ "ETH", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "faee959898c691c89fc7497f3ab5ab08c2cc7865f48064b803781efa8958cd65", "ETH反馈" ]' -p ethethetheth

cleos push action usdt.bos feedback '[ "USDT", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "faee959898c691c89fc7497f3ab5ab08c2cc7865f48064b803781efa8958cd65", "USDT反馈" ]' -p usdtusdtusdt
```

# 十六、rollback【通知：notify_plugin】

功能：  从 BOS 链向其他链换币失败时，承兑商反馈信息。
此时 state 设为 5。
调用 notify 插件接口通知该账号, 将来前端可以监听到并提示。
设置 defer，删除满足条件的记录。
即满足任意一点：
(a) 提币记录，距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 state=2（提币成功）或者 state=3（提币已退回）  
(b) 提币记录，距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 quantity 小于设定的 单次最小提币金额

权限：承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth
参数：

```
symbol_code sym_code  //币名。如BTC、ETH
transaction_id_type trx_id  //BOS链上提币的交易号
string memo  //备注
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
withdraws 表的 enable 必须为 true，并且 state 必须等于 0，才可执行。
memo 可以为空。
将 state 置为 5.
调用 notify 插件通知用户。可以根据 trx_id 查询 withdraws 表得到用户名。
设置 defer，定时删除符合条件的记录。

```
cleos push action btc.bos rollback '[ "BTC", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2",  "BTC回滚" ]' -p btcbtcbtcbtc

cleos push action eth.bos rollback '[ "ETH", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2",  "ETH回滚" ]' -p ethethetheth

cleos push action usdt.bos rollback '[ "USDT", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2",  "USDT回滚" ]' -p usdtusdtusdt
```

# 十七、sendback

功能： 提币失败之后，承兑商将币退回原账号。
此时 state=3。
账号必须真实存在。
不能是承兑商的相关账户，包括所有审核员账号及相应币种承兑人的账号，如 boshuobipool、btcbtcbtcbtc、ethethetheth、huobiauditor。也不能是合约部署账号，如 btc.bos。
调用 notify 插件接口通知该账号, 将来前端可以监听到并提示。

权限：承兑商审核员账号
参数：

```
name auditor //审核员账号
transaction_id_type trx_id  //BOS链上提币的交易号
name    to  //退回的账号
asset   quantity  //需要退回的资产总数
string  memo  //备注
```

大致流程：
校验权限。
根据 quantity 的 sym_code 确定 scope。
交易 to 是否为合法的 BOS 账号。
交易 to 是否为上述特殊账号。
quantity 必须大于 0。
withdraws 表的 enable 必须为 true，并且 state 必须等于 5，才可执行。
memo 可以为空；若不为空，则填入 withdraws 表的 msg 字段。可以根据 trx_id 查询 withdraws 表得到用户名。
将 state 置为 3.
给该用户转相应数量的资产。
调用 notify 插件通知用户。

```
cleos push action btc.bos sendback '[ "huobiauditor", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "huobihuaping",  "1.00000001 BTC", "BTC退回" ]' -p btcbtcbtcbtc

cleos push action eth.bos sendback '[ "huobiauditor", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "huobihuaping", "1.00000001 ETH", "ETH退回" ]' -p ethethetheth

cleos push action usdt.bos sendback '[ "huobiauditor", "5b641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "huobihuaping", "1.00000001 USDT", "USDT退回" ]' -p usdtusdtusdt
```

# 十八、lockall

功能：  承兑商锁定服务。
此时所有的提币、充币、转账都无法进行。
权限：承兑商审核员账号
参数：

```
symbol_code sym_code  //币名。如BTC、ETH、USDT
name auditor //审核员账号
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
修改 stat 的 active 字段为 false。

```
cleos push action btc.bos lockall '["BTC","huobiauditor"]' -p huobiauditor
cleos push action eth.bos lockall '["ETH","huobiauditor"]' -p huobiauditor
cleos push action usdt.bos lockall '["USDT","huobiauditor"]' -p huobiauditor
```

# 十九、unlockall

功能：  承兑商恢复服务。
此时所有的提币、充币、转账都恢复正常。
权限：承兑商审核员账号
参数：

```
symbol_code sym_code  //币名。如BTC、ETH、USDT
name auditor //审核员账号
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
修改 stat 的 active 字段为 true。

```
cleos push action btc.bos unlockall '["BTC","huobiauditor"]' -p huobiauditor
cleos push action eth.bos unlockall '["ETH","huobiauditor"]' -p huobiauditor
cleos push action usdt.bos unlockall '["USDT","huobiauditor"]' -p huobiauditor
```

# 二十、approve 【auditor】

功能：  承兑商审核通过某笔提币
权限：承兑商任一审核员账号
参数：

```
symbol_code sym_code  //币名。如BTC、ETH
name auditor //审核员账号
transaction_id_type trx_id  //BOS链上提币的交易号
string memo  //备注
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
根据 trx_id 查询 withdraws 表，判断 auditor 字段是否存在，若为空，则修改 withdraws 表的 enable 字段为 true、auditor 字段为当前的 auditor；若 auditor 字段不为空，则不处理。
memo 可以为空。若不为空，则填入 withdraws 表的 msg 字段。

```
cleos push action btc.bos approve '["BTC","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "BTC审核同意"]' -p huobiauditor

cleos push action eth.bos approve '["ETH","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "ETH审核同意"]' -p huobiauditor

cleos push action usdt.bos approve '["USDT","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "USDT审核同意"]' -p huobiauditor
```

# 二十一、unapprove

功能：  承兑商拒绝通过某笔提币
权限：承兑商任一审核员账号
参数：

```
symbol_code sym_code  //币名。如BTC、ETH
name auditor //审核员账号
transaction_id_type trx_id  //BOS链上提币的交易号
string memo  //备注
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
根据 trx_id 查询 withdraws 表，判断 auditor 字段是否存在，若为空，则修改 withdraws 表的 enable 字段为 false、auditor 字段为当前的 auditor；若 auditor 字段不为空，则不处理。
memo 可以为空。若不为空，则填入 withdraws 表的 msg 字段。

```
cleos push action btc.bos unapprove '["BTC","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "BTC审核拒绝"]' -p huobiauditor

cleos push action eth.bos unapprove '["ETH","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "ETH审核拒绝"]' -p huobiauditor

cleos push action usdt.bos unapprove '["USDT","huobiauditor", "5641b15168df9ce49295c694ed9e77608b0708bc60238b100eb483ec481ead2", "USDT审核拒绝"]' -p huobiauditor
```

# 二十二、clear

功能：承兑商清理满足条件的换币记录, 包括充币和提币。
即满足任意一点：
(a) 提币记录（withdraws 表），距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 state=2（提币成功）或者 state=3（提币已退回）  
(b)  提币记录（withdraws 表）， 距离 create_time 的间隔超过指定时间（比如 7 天）， 并且 quantity 小于设定的单次最小提币金额
 (c) 充币记录（deposits 表），距离 create_time 的间隔超过指定时间（比如 7 天）
权限：承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth
参数：

```
symbol_code sym_code  //币名。如BTC、ETH
uint64_t num   //删除条数。 默认为1条。按时间排序先删最老的记录。
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
查找 withdraws 和 deposits 中符合条件的记录，删除指定条数。默认 1 条。

```
cleos push action btc.bos clear '[ "BTC", "2" ]' -p btcbtcbtcbtc
cleos push action eth.bos clear '[ "ETH", "3" ]' -p ethethetheth
cleos push action usdt.bos clear '[ "USDT", "3" ]' -p usdtusdtusdt
```

# 二十三、 setauditor

功能：承兑商添加或者删除自己的审核员。
可以设置多个审核员。
不能是承兑商的相应币种承兑人的账号，如 btcbtcbtcbtc、ethethetheth。也不能是合约部署账号，如 btc.bos。

权限：承兑商账号, 如 boshuobipool
参数：

```
symbol_code sym_code //币名
string  action          //动作：添加或删除     字符串为 add 或 remove
name    auditor     //承兑商的某个审核员账户
```

大致流程：
校验权限。
根据 sym_code，在 auditors 表中增加或删除一条记录。
action 为"add"时，增加记录；action 为"remove"时，删除记录。
需要判断账号是否已经绑定了地址、是否有余额，如果满足任意一条，则报错。

```
cleos push action btc.bos setauditor '["BTC","add","huobiauditor"]' -p boshuobipool
cleos push action btc.bos setauditor '["BTC","remove","huobiauditor"]' -p boshuobipool

```

# 二十四、 setacceptor

功能：承兑商添加或者修改相应币种的承兑人。
必须设置且只能设置一个承兑人。
不能是承兑商的审核员的账号，如 huobiauditor。也不能是合约部署账号，如 btc.bos。
权限：承兑商账号, 如 boshuobipool。
参数：

```
symbol_code sym_code //币名
name    acceptor     //承兑商的承兑人账户
```

大致流程：
校验权限。
根据 sym_code 确定 scope。
修改 stat 的 acceptor 字段。
需要判断新的 acceptor 是否绑定了地址，若有，则报错。
需要判断原有的 acceptor 是否还有余额，如果有，则报错。

```
cleos push action btc.bos setacceptor '["BTC","btcbtcbtcbtc"]' -p boshuobipool
cleos push action eth.bos setacceptor '["ETH","ethethetheth"]' -p boshuobipool
cleos push action usdt.bos setacceptor '["USDT","usdtusdtusdt"]' -p boshuobipool
```

# 二十五、 setvip

功能：承兑商添加或者修改相应币种的某个用户为 vip 用户。
不能是承兑商的审核员的账号，如 huobiauditor。不能是承兑商账户，如 boshuobipool。也不能是合约部署账号，如 btc.bos。
权限：承兑商的承兑人账号, 如 btcbtcbtcbtc。
参数：

```
symbol_code sym_code //币名
string  action          //动作：添加或删除     字符串为 add 或 remove
name    vip     //vip用户账户
```

大致流程：
校验权限。
根据 sym_code 确定 scope, 在 vips 表中增加或删除一条记录。
action 为"add"时，增加记录；action 为"remove"时，删除记录。

```
cleos push action btc.bos setvip '["BTC", "add", "huobihuaping"]' -p boshuobipool
cleos push action eth.bos setvip '["ETH", "add", "huobihuaping"]' -p boshuobipool
cleos push action usdt.bos setvip '["USDT","add", "huobihuaping"]' -p boshuobipool
```

# 二十六、verify_address 【工具类】

功能：  校验 BTC/ETH 的主网、测试网的地址有效性
合约内部工具函数。
由于某些币种（如 BTC）主网和测试网地址规则不同，此函数由宏编译选项来控制编译相应的校验代码。测试网设为 testnet, 主网设为 mainnet。若不设置，默认为 mainnet。
权限： 无
参数：

```
name style  //地址类型。如bitcoin、ethereum
string address  //相应币种地址
```

大致流程：
根据 style 并且根据编译选项编译不同代码，区分测试网和主网。
只支持标准的 BTC 和 ETH 地址。不标准的地址不提供服务，不迁就不标准的地址。
地址校验尽可能精确，避免无谓的提币失败和退款。

#

# 二十七、verify_txid【工具类】

功能：  校验其他链的交易号有效性
合约内部工具函数。
权限： 无
参数：

```
name style  //地址类型。如bitcoin、ethereum
string txid  //其他链的交易号
```

大致流程：
根据 style，校验 txid 是否为合法的交易号。

# 第三部分：表结构设计

# 表 symbols

    介绍：币种表

每个币一条记录。
字段：

```
symbol  sym  //币种，如ETH、BTC等。主键
```

# 表 addrs [币种]

    介绍：账号地址映射表

每个账号一条记录。
scope: 币种 如 BTC、ETH、BTCZB、ETHZB
字段：

```
	name  owner  //普通用户账号。   主键
	string  address //其他链的地址。第二索引
	uint64_t  state //状态值。未分配地址时为非0，取值为owner.value；分配地址后为0。 第三索引。
	time_point_sec    create_time  //申请地址时间
	time_point_sec   assign_time   //地址映射时间
```

```
cleos get table btc.bos BTC addrs
cleos get table eth.bos ETH addrs
```

# 表 operates [币种]

    介绍：承兑商发行或销毁币的记录

每次发行或销毁，增加一条记录
scope: 币种 例如：BTC， BTCSG

    字段：

```
	uint64_t id  //序号。主键。
	name     to //承兑商相应币种承兑人的账户,如btcbtcbtcbtc、ethethetheth
	asset    quantity //发行或销毁的资产总额
    uint64_t    type  //0：销毁 1：发行
	string   memo  //备注
	time_point_sec   operate_time   //操作时间
```

```
cleos get table btc.bos BTC operates
cleos get table eth.bos ETH operates
```

# 表 withdraws [币种]

    介绍：用户提币记录

scope: 币种 如 BTC、ETH
字段：

```
	uint64_t             id  //序号。主键。
	transaction_id_type   trx_id  //BOS链上的交易号。第二索引
	name              from  //普通用户账号
    string               to  //BTC地址、ETH地址
	asset                quantity //本次提币金额
	uint64_t             state //状态值。提币成功为2；提币退回为3；提币回滚为5。 第三索引。
	bool                enable //是否放行。审核员拒绝时，置为false;审核员同意时，置为true；其他情况默认置为true。
    name            auditor //审核员账号。审核这笔提币的审核员账号。默认为空。
	string               remote_trx_id //其他链的交易号
	string               msg //备注信息
	time_point_sec       create_time //申请提币时间
	time_point_sec       update_time //提币成功、提币回滚、提币退回的时间、审核员审核时间
```

```
cleos get table btc.bos BTC withdraws
cleos get table eth.bos ETH withdraws
```

# 表 deposits [币种]

    介绍：用户充币记录

scope: 币种 如 BTC、ETH、USDT
字段：

```
	uint64_t             id //序号。主键。
	transaction_id_type  trx_id  //BOS链上的交易号。第二索引
	name                 to  //普通用户账号
    string               from  //BTC地址、ETH地址
	asset                quantity //充币总额
	uint64_t             state //状态值。第三索引。预留。先统一设为0。
	string               remote_trx_id //其他链的交易号
	string               msg //备注信息
	time_point_sec       create_time //充币时间
	time_point_sec       update_time //更新时间。预留。先设为与create_time相同。
```

```
cleos get table btc.bos BTC deposits
cleos get table eth.bos ETH deposits
```

# 表 statistics [币种]

介绍：用户的提币信息统计
scope: 币种 如 BTC、ETH
字段：

```
    name  owner  //普通用户账号。   主键
    time_point_sec   last_time  //上次提币时间
    uint64_t  frequency //上一个自然日零点开始到提币申请时的提币次数。每日零点之后重置
    asset   total //上一个自然日零点开始到提币申请时的提币总额。每日零点之后重置
	time_point_sec       update_time    //统计时间
```

```
cleos get table btc.bos BTC statistics
cleos get table eth.bos ETH statistics
```

# 表 accounts [用户]

    介绍：账号信息表

每个账号一条记录
scope: 用户名
字段：

```
	asset    balance //账号拥有的该币种的余额。主键。
```

```
cless get table btc.bos huobihuaping accounts
cleos get table eth.bos huobiliqiang accounts
```

# 表 vips [用户]

    介绍：VIP账号信息表

每个账号一条记录
scope: 币种 如 BTC、ETH
字段：

```
	name  owner  //vip用户账号。   主键 
    asset    maximum_limit //单次最大换币金额
	asset    minimum_limit //单次最小换币金额
    asset    total_limit //单日累计换币总额
    uint64_t frequency_limit   //单日累计换币次数
    uint64_t interval_limit  //两次换币间隔秒数
    double   service_fee_rate //服务费率
	asset    min_service_fee //服务费最低收费
    asset    miner_fee //矿工费
```

```
cless get table btc.bos BTC vips
cleos get table eth.bos ETH vips
```

# 表 stats [币种]

    介绍：币种信息表

scope: 币种 如 BTC、ETH、USDT
字段：

```
	asset    supply //该币种发行量。  币种名为主键。 链上所有账号（含承兑人账号）的余额之和等于发行量。
	asset    maximum_limit //单次最大换币金额
	asset    minimum_limit //单次最小换币金额
    asset    total_limit //单日累计换币总额
    uint64_t frequency_limit   //单日累计换币次数
    uint64_t interval_limit  //两次换币间隔秒数
    uint64_t delay    //记录保存天数  默认为7，最小为1
	name     issuer //承兑商账号  非空
    name     acceptor //承兑商相应币种承兑人账号  非空
    string     address_style; //币的类型 如：bitcoin、ethereum、tether
	string   organization //承兑商简介
	string   website //承兑商官网
	double   service_fee_rate //服务费率
	asset    min_service_fee //服务费最低收费
    asset    miner_fee //矿工费
	bool     active //是否正常提供服务
```

```
cleos get table btc.bos BTC stats
cleos get table eth.bos ETH stats
```

# 表 auditors

    介绍：承兑商的审核员

每个审核员一条记录。
scope: 币种 如 BTC、ETH、USDT
字段：

```
name  auditor  //审核员账号。主键
```

```
cleos get table btc.bos BTC auditors
cleos get table eth.bos ETH auditors
```
