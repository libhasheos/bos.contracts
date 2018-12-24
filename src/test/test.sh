# cleos wallet import    --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
# cleos wallet import    --private-key 5JLCRk2BZ3rYD2VUbB2ndhRGCV6B8vyK7vE8EDe5GDBuU33jXph
# cleos wallet import    --private-key 5K36gdYNETt5o4VgimA9RvrXqtkBMfcU3MBi3SCja2UzJrqWe85
# cleos wallet import    --private-key 5J3wAcdaTQjAKMCxjk7aaqpNmQXySi74sWn7TL9bFKgKGexnX2o
# cleos wallet import    --private-key 5Ja7dvhrsAZ6SUMHecs4W1tW13QmuVSunwgXnH2o9MS1T1nk51q
# cleos wallet import    --private-key 5JWoGYoABxdXrAUu39kfAhPjrRGQKbpVmdmSYwJobRySYobdeYJ
# cleos wallet import    --private-key 5J1VwUtdA9bS4aC2EkdCTfDqZJaXwqXwjZSdtNKbUUe5Wrs3D68
# cleos wallet import    --private-key 5JZ5qvHCn5hRX1g6cigGK8QynvxiEwkbEA6qUpxb4aLLXtNYLch

cleos   create account eosio boshuobipool  EOS6LKzKLPn3CC7sF5gwJAFqz2qwHdMCR1Ax9KFm95o8PDkxd2HKj  EOS5fzSQo4Ko9hatR7y2W8MktEHeEx5P3yTJyqZutQvR4VQHJxiFn -p eosio
cleos   create account eosio peghuobipool  EOS7ySXVfVKx7A2KhZJeA5QnPNao7hbBD6GQB9Uihn9gcNxKeREw1  EOS74RsAE5XvYUxwQF7KBQSUP6bbn39VpEcGH7HKJVMwVL7PwkcNL -p eosio
cleos   create account eosio eosio.token  EOS6Fm6PAJbu3kDEdgAny6qM4gm7pWJKzSGhoYmT78Nqhdahoco55  EOS6nGWQ56pK1zpAge2M6vJhXRKs1wYBN2i7V4R4nVzNAiVCDRgdR -p eosio
cleos   create account eosio huobiauditor  EOS8XzK82ZGT8j3asLnrsUTfZgwUK9mtHkAWSmzYDgSjGKnZhpYbY  EOS7rTRVU42HWYidQsAFRYqm13LkXhjUHfxDE9sZCSuiZt7DW2xew -p eosio
cleos   create account eosio btchuobipool  EOS7oP3Rvhv5tifTeK7hr51o3Pzj7h1crEmABLKgPXZRdTPp4mZLr  EOS5x8ssY9Kfva3Vvi43CrNTvLSBgb8fFuAwPnVpx7xJ1Dkh6wQBv -p eosio
cleos   create account eosio eoshuobipool  EOS6EKZy7xiMk3dSaHp5jnYusH9ye5ZAWc2CmX44aFvwM2LKcGsvZ  EOS5wAnt4zUQuWBQgivsXnuypbaYqecv2Uwc1hC5nsD19rXsKU2rD -p eosio
cleos   create account eosio huobipartner  EOS7FGnPuxf4KK8oWbdC2FMcbk6U1z8c4yKg2PsKKuPDnzp8DGqdd  EOS8meUWqwEa9jzCqDyhV7Gt99Ghn1wcj5qjmY8rfJDtHx4u3nqxN -p eosio
cleos   create account eosio huobiliqiang  EOS58Ng2PFgVXgzuZF3dcmo3pe1tunsRTzmoNDNWGoWGA1dbkMGPr  EOS8XDV4iCBVXnFfy7JwYjRzncVACGA4uvMCnc4mai6arrinCtjMZ -p eosio
cleos   create account eosio huobihuaping  EOS5AfG5VZ9QZzJeQuZkqSR5fEUqZ9XKLnD3vGqDnVK5AXeMmgJFx  EOS7RhPVtAectGtTk4Pzpofajn47PE4zGs5bWhy7gYooVpg3uZFKZ -p eosio
cleos   create account eosio simbahebinbo  EOS6g5RRLKT8GbjhjiSyDGW6dHMwB5v7bGvBBZbJ1HuHviCG4x9jx  EOS6PJVB25TJqUpb7h3RLwYZdjg5X6zDPHDxnnUBkEu3kBj3n5akU -p eosio
cleos   create account eosio ethhuobipool  EOS7B6k4xpss9KBLqReUnjutSqymXPFzxfH6qMHucAKpFT1jGLZvK  EOS5xYbmJTZzhj94uyNUYnkp3peC5MPYvfapZNY1eMf2oQuHMWRq8 -p eosio
cleos   create account eosio bos.pegtoken  EOS5gSCnofifvaQ363g2y61FQxFfbB7ky6oYDMFh4sTog4QbmDYXs  EOS82ijhByZmhquyczSMdX7CLPC6dGGanvfeiXk77yD9iKCM9NtdT -p eosio


cleos set contract bos.pegtoken /bos.contracts/src/bos.pegtoken -p bos.pegtoken

cleos  push action bos.pegtoken create '[ "peghuobipool", "8,BTC"]' -p bos.pegtoken
cleos  push action bos.pegtoken create '[ "peghuobipool", "8,ETH"]' -p bos.pegtoken
cleos  push action bos.pegtoken create '[ "peghuobipool", "4,EOS"]' -p bos.pegtoken

cleos  push action bos.pegtoken init '[ "BTC", "火币矿池", "http://www.huobipool.com", "btchuobipool"]' -p peghuobipool
cleos  push action bos.pegtoken init '[ "ETH", "火币矿池", "http://www.huobipool.com", "ethhuobipool"]' -p peghuobipool
cleos  push action bos.pegtoken init '[ "EOS", "火币矿池", "http://www.huobipool.com", "eoshuobipool"]' -p peghuobipool

cleos  push action bos.pegtoken update '[ "BTC", "huobipool", "http://www.poolhuobi.com" ]' -p peghuobipool
cleos  push action bos.pegtoken update '[ "ETH", "huobipool", "http://www.poolhuobi.com" ]' -p peghuobipool
cleos  push action bos.pegtoken update '[ "EOS", "huobipool", "http://www.poolhuobi.com" ]' -p peghuobipool

cleos  push action bos.pegtoken setpartner '[ "BTC", "add", "huobipartner" ]' -p peghuobipool
cleos  push action bos.pegtoken setpartner '[ "ETH", "add", "huobipartner" ]' -p peghuobipool
cleos  push action bos.pegtoken setpartner '[ "EOS", "add", "huobipartner" ]' -p peghuobipool

cleos  push action bos.pegtoken applyaddr '[ "huobipartner", "BTC", "huobiliqiang" ]' -p huobipartner
cleos  push action bos.pegtoken applyaddr '[ "huobipartner", "ETH", "huobiliqiang" ]' -p huobipartner
cleos  push action bos.pegtoken applyaddr '[ "huobipartner", "EOS", "huobiliqiang" ]' -p huobipartner

cleos  push action bos.pegtoken issue '[ "50.00000000 BTC", "发行BTC" ]' -p peghuobipool
cleos  push action bos.pegtoken issue '[ "50.00000000 ETH", "发行ETH" ]' -p peghuobipool
cleos  push action bos.pegtoken issue '[ "50.0000 EOS", "发行EOS" ]' -p peghuobipool

cleos push action bos.pegtoken assignaddr '[ "BTC", "huobiliqiang", "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB" ]' -p btchuobipool

cleos push action bos.pegtoken deposit '[  "huobihuaping", "0.90000000 BTC", "BTC充币" ]' -p btchuobipool
cleos push action bos.pegtoken deposit '[  "huobiliqiang", "0.80100000 BTC", "BTC充币" ]' -p btchuobipool

cleos push action bos.pegtoken transfer '[ "huobihuaping", "huobiliqiang", "0.40300000 BTC", "BTC转账" ]' -p huobihuaping

cleos push action bos.pegtoken withdraw '[ "huobihuaping", "3JQSigWTCHyBLRD979JWgEtWP5YiiFwcQB", "0.10000000 BTC", "BTC提币" ]' -p huobihuaping

cleos push action bos.pegtoken setauditor  '["BTC","add","huobiauditor"]' -p peghuobipool
cleos push action bos.pegtoken approve '["BTC","huobiauditor", "a4f5f2f6c5fa150d8c32a266927f4f4e2b8750d7645e3d992a44571d64236ac3", "BTC审核同意"]' -p huobiauditor
cleos push action bos.pegtoken unapprove '["BTC","huobiauditor", "a4f5f2f6c5fa150d8c32a266927f4f4e2b8750d7645e3d992a44571d64236ac3", "BTC审核拒绝"]' -p huobiauditor

cleos push action bos.pegtoken retire '["10.00100000 BTC","销毁BTC"]' -p peghuobipool
cleos push action bos.pegtoken feedback '[ "BTC", "a4f5f2f6c5fa150d8c32a266927f4f4e2b8750d7645e3d992a44571d64236ac3", "faee959898c691c89fc7497f3ab5ab08c2cc7865f48064b803781efa8958cd65", "BTC反馈" ]' -p btchuobipool

cleos push action bos.pegtoken rollback '[ "BTC", "a4f5f2f6c5fa150d8c32a266927f4f4e2b8750d7645e3d992a44571d64236ac3",  "BTC回滚" ]' -p btchuobipool


cleos push action bos.pegtoken lockall '[ "BTC", "huobiauditor" ]' -p huobiauditor
cleos push action bos.pegtoken unlockall '[ "BTC", "huobiauditor" ]' -p huobiauditor
