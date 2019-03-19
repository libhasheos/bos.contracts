#include <bos.pegtoken/bos.pegtoken.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/action.hpp>
#include "def.cpp"

namespace eosio {

////////////////////////
// actions
////////////////////////

    void pegtoken::create( symbol sym, name issuer, name address_style, uint64_t peg ) {
        eosio_assert(peg == peg_type::PRE_RELEASE || peg == peg_type::STRICT_ANCHOR, "peg can only be 1 or 2");
        require_auth(get_self());
        ACCOUNT_CHECK(issuer);
        eosio_assert(sym.is_valid(), "invalid symbol");
        eosio_assert(address_style == "bitcoin"_n || address_style == "ethereum"_n || 
                     address_style == "tether"_n || address_style == "other"_n,
                     "address_style must be one of bitcoin, ethereum, tether or other");

        auto sym_code = sym.code();
        auto sym_raw = sym_code.raw();

        auto symbol_table = symbols(get_self(), sym_raw);
        symbol_table.emplace(get_self(), [&](auto &p) {
            p.sym = sym;
        });

        auto info_table = infos(get_self(), sym_raw);
        eosio_assert(info_table.find(sym_raw) == info_table.end(), "token with symbol already exists (info)");

        info_table.emplace(get_self(), [&] (auto &p) {
            p.supply = eosio::asset(0, sym);
            p.issuer = issuer;
            p.address_style = address_style;
            p.active = true; 
        });

        auto summary_table = summaries(get_self(), sym_raw);
        eosio_assert(summary_table.find(sym_raw) == summary_table.end(), "token with symbol already exists (summary)");
        summary_table.emplace(get_self(), [&](auto &p) {
            /* do nothing */
        });

        // Init limits.
        auto limit_table = limits(get_self(), sym_raw);
        eosio_assert(limit_table.find(sym_raw) == limit_table.end(), "token with symbol already exists (limit)");
        limit_table.emplace(get_self(), [&](auto &p) {
            p.maximum_limit = eosio::asset(MAXIMUM_LIMIT, sym);
            p.minimum_limit = eosio::asset(MINIMUM_LIMIT, sym);
            p.total_limit = eosio::asset(TOTAL_LIMIT, sym);

            p.frequency_limit = FREQUENCY_LIMIT;
            p.interval_limit = INTERVAL_LIMIT;
            p.reset_limit = 30 * ONE_DAY;
        });

        // Init fees.
        auto fee_table = fees(get_self(), sym_raw);
        eosio_assert(fee_table.begin() == fee_table.end(), "token with symbol already exists (fee)");
        fee_table.emplace(get_self(), [&](auto &p) {
            p.service_fee_rate = SERVICE_FEE_RATE;
            p.min_service_fee = eosio::asset(MIN_SERVICE_FEE, sym);
            p.miner_fee = eosio::asset(MINER_FEE, sym);
        }); 
    }

    void pegtoken::setissuer( symbol_code sym_code, name issuer )
    {
        require_auth(get_self());
        ACCOUNT_CHECK(issuer);

        auto sym_raw = sym_code.raw();
        auto info_table = infos(get_self(), sym_raw);
        auto iter = info_table.find(sym_raw);
        eosio_assert(iter != info_table.end(), "token not exist");
        
        info_table.modify(iter, same_payer, [&](auto &p) {
            p.issuer = issuer;
        });
    }

    void pegtoken::setlimit( symbol_code sym_code, asset maximum_limit, asset minimum_limit,
        asset total_limit, uint64_t frequency_limit, uint64_t interval_limit ) {
        is_auth_manager(sym_code);
        eosio_assert(is_sym_equal_asset(sym_code, maximum_limit), "sym_code is not same as maximum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, minimum_limit), "sym_code is not same as minimum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, total_limit), "sym_code is not same as total_limit symbol_code.");
        
        eosio_assert(minimum_limit.amount >= 0 && maximum_limit >= minimum_limit && total_limit >= maximum_limit,
             "constrict mismatch: total_limit >= maximum_limit >= minimum_limit >= 0");
        is_auth_manager(sym_code);
        auto limit_table = limits(get_self(), sym_code.raw());
        auto iter = limit_table.find(sym_code.raw());
        eosio_assert( iter != limit_table.end(), "Token with symbol not exists(limit)");
        limit_table.modify(iter, same_payer, [&](auto &p) {
            p.maximum_limit = maximum_limit;
            p.minimum_limit = minimum_limit;
            p.total_limit = total_limit;
            p.frequency_limit = frequency_limit;
            p.interval_limit = interval_limit;
        });
    }

    void pegtoken::setviplimit( symbol_code sym_code, name vip, asset maximum_limit,asset minimum_limit,
        asset total_limit, uint64_t frequency_limit, uint64_t interval_limit ) {
        is_auth_manager(sym_code);
        eosio_assert(is_sym_equal_asset(sym_code, maximum_limit), "sym_code is not same as maximum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, minimum_limit), "sym_code is not same as minimum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, total_limit), "sym_code is not same as total_limit symbol_code.");
        
        eosio_assert( maximum_limit >= minimum_limit && total_limit >= maximum_limit && minimum_limit.amount >= 0, "mismatch: total >= max >= min >= 0");
        auto sym_raw = maximum_limit.symbol.code().raw();

        auto vip_table = vips(get_self(), sym_raw);
        auto viplimit_table = viplimits(get_self(), sym_raw);
        auto vipfee_table = vipfees(get_self(), sym_raw);
        
        auto iter_vip = vip_table.find(vip.value);
        eosio_assert(iter_vip != vip_table.end(), "vip not set.");

        viplimit_table.modify(viplimit_table.find(vip.value), same_payer, [&](auto &p) {
            p.owner = vip;
            p.maximum_limit = maximum_limit;
            p.minimum_limit = minimum_limit;
            p.total_limit = total_limit;
            p.frequency_limit = frequency_limit;
            p.interval_limit = interval_limit;
        });
    }

    void pegtoken::setfee( symbol_code sym_code, double service_fee_rate,
        asset min_service_fee, asset miner_fee ) {
        eosio_assert(is_sym_equal_asset(sym_code, min_service_fee), "sym_code is not same as min_service_fee symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, miner_fee), "sym_code is not same as miner_fee symbol_code.");
        is_auth_manager(sym_code);
        
        eosio_assert(min_service_fee.symbol == miner_fee.symbol, "different symbol");
        auto sym_raw = sym_code.raw();

        auto fee_table = fees(get_self(), sym_raw);
        eosio_assert(fee_table.begin() != fee_table.end(), "fees table not initialized.");
        fee_table.modify(fee_table.begin(), same_payer, [&](auto &p) {
            p.service_fee_rate = service_fee_rate;
            p.min_service_fee = min_service_fee;
            p.miner_fee = miner_fee;
        });
    }

    void pegtoken::setvipfee( symbol_code sym_code, name vip, double service_fee_rate, asset min_service_fee, asset miner_fee ) {
        is_auth_manager(sym_code);
        eosio_assert(is_sym_equal_asset(sym_code, min_service_fee), "sym_code is not same as min_service_fee symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, miner_fee), "sym_code is not same as miner_fee symbol_code.");
        
        eosio_assert(min_service_fee.symbol == miner_fee.symbol, "invalid symbol");
        auto sym_raw = min_service_fee.symbol.code().raw();

        auto vip_table = vips(get_self(), sym_raw);
        auto vipfee_table = vipfees(get_self(), sym_raw);
        
        auto iter_vip = vip_table.find(vip.value);
        eosio_assert(iter_vip != vip_table.end(), "not in the VIP table");
        if(iter_vip != vip_table.end()) {
            vipfee_table.modify(vipfee_table.find(vip.value), same_payer, [&](auto &p) {
                p.service_fee_rate = service_fee_rate;
                p.min_service_fee = min_service_fee;
                p.miner_fee = miner_fee;
            });
        }
    }

    void pegtoken::setcheck( symbol_code sym_code, bool in_check, bool out_check ) {
        is_auth_manager(sym_code);
        
        auto check_tb = checks(get_self(), sym_code.raw());
        if(check_tb.begin() == check_tb.end()) {
            check_tb.emplace(get_self(), [&](auto &p) {
                p.sym = symbol(sym_code, SYMBOL_PRECISION);
                p.in_check = in_check;
                p.out_check = out_check;
            });
        } else {
            check_tb.modify(check_tb.begin(), same_payer, [&](auto &p) {
                p.in_check = in_check;
                p.out_check = out_check;
            });
        }
    }

    void pegtoken::precast(symbol_code sym_code, string to_address, name to_account, 
        string remote_trx_id, asset quantity, uint64_t index, string memo) {
        is_auth_teller(sym_code);
        is_auth_role(sym_code, to_account);
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        
        ACCOUNT_CHECK(to_account);
        STRING_LEN_CHECK(memo, 256);
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        
        auto sym_raw = quantity.symbol.code().raw();
        auto addr_table = addrs(get_self(), sym_raw);
        auto iter_addr = addr_table.find(to_account.value);
        eosio_assert(iter_addr != addr_table.end() && iter_addr->address == to_address, "invalid to_address");

        auto cast_table = casts(get_self(), sym_raw);
        auto index_str = to_address + to_account.to_string() + remote_trx_id + std::to_string(index) + quantity.to_string();
        auto id = hash64(index_str);
        eosio_assert(cast_table.find(id) == cast_table.end(), "Already have the same precast hash.");

        cast_table.emplace(get_self(), [&](auto &p) {
            p.id = id;
            p.to_account = to_account;
            p.to_address = to_address;
            p.quantity = quantity;
            p.state = 0;
            p.need_check = true;
            p.enable = false;
            p.auditor = NIL_ACCOUNT;
            p.remote_trx_id = remote_trx_id;
            p.remote_index = index;
            p.msg = memo;
            p.create_time = time_point_sec(now());
            p.update_time = time_point_sec(now());
        });
    }

    void pegtoken::agreecast( symbol_code sym_code, string to_address, name to_account,
        name auditor, string remote_trx_id, asset quantity, uint64_t index, string memo ) {
        is_auth_auditor(sym_code);
        is_auth_role(sym_code, to_account);
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        
        ACCOUNT_CHECK(to_account);
        STRING_LEN_CHECK(memo, 256);
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        
        auto sym_raw = quantity.symbol.code().raw();

        auto addr_table = addrs(get_self(), sym_raw);
        auto iter_addr = addr_table.find(to_account.value);
        eosio_assert(iter_addr != addr_table.end() && iter_addr->address == to_address, "invalid to_address");

        auto cast_table = casts(get_self(), sym_raw);
        auto index_str = to_address + to_account.to_string() + remote_trx_id + std::to_string(index) + quantity.to_string();
        auto iter_cast = cast_table.find(hash64(index_str));
        eosio_assert(iter_cast != cast_table.end()
            && iter_cast -> to_account == to_account
            && iter_cast -> to_address == to_address
            && iter_cast -> remote_trx_id == remote_trx_id
            && iter_cast -> remote_index == index
            && iter_cast -> state == cast_state::CAST_INIT
            && iter_cast -> quantity == quantity
            , "invalid cast");

        auto info_table = infos(get_self(), sym_raw);
        auto iter_info = info_table.find(sym_raw);
        eosio_assert(iter_info != info_table.end(), "token not exist in infos table");

        // Add balance of to_account
        add_balance(to_account, quantity, iter_info->issuer);
        // Add supply of infos
        info_table.modify(iter_info, same_payer, [&] (auto &p) {
            p.supply += quantity;
        });
        auto auditor_tb = auditors(get_self(), sym_code.raw());
        auto auditor_val = auditor_tb.get(sym_code.raw(), "the v2 token NOT in auditors table");
        cast_table.modify(iter_cast, same_payer, [&] (auto &p) {
            if (p.need_check && !p.enable) {
                p.enable = true;
            }
            p.trx_id = get_trx_id();
            p.index = 0;
            p.msg = memo;
            p.auditor = auditor;
            p.state = cast_state::CAST_SUCCESS;
            p.update_time = time_point_sec(now());
            p.auditor = auditor_val.auditor;
        });
    }

    void pegtoken::refusecast( symbol_code sym_code, string to_address, name to_account,
        name auditor, string remote_trx_id, asset quantity, uint64_t index, string memo ) {
        is_auth_auditor(sym_code);
        is_auth_role(sym_code, to_account);
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        
        ACCOUNT_CHECK(to_account);
        STRING_LEN_CHECK(memo, 256);
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        
        auto sym_raw = quantity.symbol.code().raw();
        auto infos_tb = infos(get_self(), sym_raw);
        auto iter_info = infos_tb.find(sym_raw);
        
        auto addr_table = addrs(get_self(), sym_raw);
        auto iter_addr = addr_table.find(to_account.value);
        eosio_assert(iter_addr != addr_table.end() && iter_addr->address == to_address, "invalid to_address");

        auto cast_table = casts(get_self(), sym_raw);
        auto index_str = to_address + to_account.to_string() + remote_trx_id + std::to_string(index) + quantity.to_string();
        auto iter_cast = cast_table.find(hash64(index_str));
        eosio_assert(iter_cast != cast_table.end()
            && iter_cast -> to_account == to_account
            && iter_cast -> to_address == to_address
            && iter_cast -> remote_trx_id == remote_trx_id
            && iter_cast -> remote_index == index
            && iter_cast -> quantity == quantity
            && iter_cast -> state == cast_state::CAST_INIT
            , "invalid cast");

        cast_table.modify(iter_cast, same_payer, [&](auto &p) {
            if (p.need_check) {
                p.enable = false;
            }
            p.trx_id = get_trx_id();
            p.index = 0;
            p.state = cast_state::CAST_FAIL;
            p.msg = memo;
            p.auditor = auditor;
            p.update_time = time_point_sec(now());
        });
    }

    void pegtoken::cast(symbol_code sym_code, string to_address, name to_account, string remote_trx_id, asset quantity,  uint64_t index, string memo){
        is_auth_role(sym_code,to_account);
        // 判断是否已经锁定
        eosio_assert(is_locked(sym_code),"The token has been locked");
        // 判断资金流入是否需要审核
        eosio_assert(!getincheck(sym_code), "This action require in_check to be false");
        ACCOUNT_CHECK(to_account);
        STRING_LEN_CHECK(memo, 256);
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        
        auto sym_raw = quantity.symbol.code().raw();
        auto info_table = infos(get_self(),sym_raw);
        auto iter_info = info_table.find(sym_raw);
        eosio_assert(iter_info != info_table.end(), "token not exist");
        require_auth(iter_info->issuer);

        auto addr_table = addrs(get_self(), sym_raw);
        auto iter_addr = addr_table.find(to_account.value);
        eosio_assert(iter_addr != addr_table.end() && iter_addr->address == to_address, "invalid to_address");

        auto cast_table = casts(get_self(), sym_raw);
        auto index_str = to_address + to_account.to_string() + remote_trx_id + std::to_string(index) + quantity.to_string();
        auto iter_cast = cast_table.find(hash64(index_str));
        eosio_assert(iter_cast != cast_table.end()
        && iter_cast -> to_account == to_account
        && iter_cast -> to_address == to_address
        && iter_cast -> remote_trx_id == remote_trx_id
        && iter_cast -> index == index
        && iter_cast -> quantity == quantity
         , "invalid cast");

        cast_table.modify(iter_cast,same_payer,[&](auto &p){
            p.trx_id = get_trx_id();
            p.msg = memo;
            p.update_time = time_point_sec(now());
        });

        add_balance(to_account, quantity, iter_info->issuer);
        info_table.modify(iter_info,same_payer,[&](auto &p){
            p.supply += quantity;
            eosio_assert(p.supply.amount >0, "supply overflow");
        });
    }
    
    void pegtoken::melt( name from_account, string to_address, asset quantity, uint64_t index, string memo ) {
        symbol_code sym_code = quantity.symbol.code();
        ACCOUNT_CHECK(from_account);
        is_auth_role_exc_gatherer(sym_code, from_account);
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(!getoutcheck(sym_code), "This action require out_check to be false");
        
        auto info_tb = infos(get_self(),sym_code.raw());
        auto info_iter = info_tb.find(sym_code.raw());
        eosio_assert(info_iter != info_tb.end(), "token not exist");
        // from_account账户余额必须大于quantity
        eosio_assert(getbalance(sym_code, from_account) > quantity, "the remaining balance of from_account should be more than quantity");
        verify_address(info_iter->address_style, to_address);
        
        eosio::asset userfee, minlimit;
        double ratelimit;
        if(is_vip(sym_code, from_account)) {
            vip_withdraw_check(sym_code, quantity, from_account);
            auto vfees_tb = vipfees(get_self(), sym_code.raw());
            auto vfee_val = vfees_tb.get(from_account.value, "This account is not in vipfees table");
            ratelimit = vfee_val.service_fee_rate;
            minlimit = vfee_val.min_service_fee;
        } else {
            withdraw_check(sym_code, quantity, from_account);
            auto fees_tb = fees(get_self(), sym_code.raw());
            auto fee_val = fees_tb.get(from_account.value, "This account is not in fees table");
            ratelimit = fee_val.service_fee_rate;
            minlimit = fee_val.min_service_fee;
        }   
        userfee = ratelimit * quantity > minlimit ? ratelimit * quantity : minlimit;
       
        action(
            permission_level{get_self(), "active"_n},
            get_self(),
            "pay"_n,
            std::make_tuple(userfee, from_account)
        ).send();

        action(
            permission_level{get_self(), "active"_n},
            get_self(),
            "ruin"_n,
            std::make_tuple(quantity - userfee, from_account)
        ).send();

        auto info_iter2 = info_tb.get(sym_code.raw(), "sym_code do not exist in infos table");
        info_tb.modify(info_iter2, same_payer, [&](auto &p) { p.supply -= (quantity-userfee); });
        
        // 当前时间，对比上次提币时间需要大于最小间隔数
        // 距离上个自然日零点到申请提币时，累计金额和次数分别需要小于相关设定：total_limit和frequency_limit

        // 以内联action的方式调用ruin, 减少from_account里面的余额
        // 减小infos表中supply的值，数值为总额减去费用。

        // 在melts添加一条记录，need_check置为false，enable字段置为false。
        auto melt_tb = melts(get_self(),  sym_code.raw());
        melt_tb.emplace(same_payer, [&](auto& mt){
            mt.id = melt_tb.available_primary_key();
            mt.trx_id = get_trx_id();
            mt.from = from_account;
            mt.to = to_address;
            mt.total = quantity;
            mt.amount = quantity - userfee;
            mt.fee = userfee;
            // mt.state = ;
            mt.need_check = false;
            mt.enable = false;
            // mt.remote_trx_id = remote_trx_id;
            // mt.remote_index = remote_index;
            // mt.auditor = get_auditor(sym_code);
            mt.auditor = get_auditor(sym_code);
            mt.msg = memo;
            mt.create_time = time_point_sec(now());
            // mt.update_time;
        });
        
        auto statistics_tb = statistics(get_self(), sym_code.raw());
        auto statistic_iter = statistics_tb.find(sym_code.raw());
        eosio_assert(statistic_iter != statistics_tb.end(), "cannot find statistic in statistics able");
        statistics_tb.modify(statistic_iter, same_payer, [&](auto &sts) {
            if(time_point_sec(now()) - statistic_iter->last_time > DAY_IN_MICROSECOND) {
                sts.frequency = 1;
                sts.total = quantity;
                sts.update_time = time_point_sec(now());
            } else {
                sts.frequency = statistic_iter->frequency + 1;
                sts.total += quantity;
                sts.update_time = time_point_sec(now());
            }
        });
    }

    // check 5 roles: deployer, teller, gatherer, manager, brakeman, issuer (check in different version)
    void pegtoken::setauditor( symbol_code sym_code, string actn, name auditor ) {
        is_auth_issuer(sym_code);
        if (actn == "add") {
            is_auth_role(sym_code, auditor);
        }
        ACCOUNT_CHECK(auditor);
        // Check if the address is already bound
        eosio_assert(addr_check(sym_code, auditor), "this account has assigned to address already");
        // Balance check
        eosio_assert(balance_check(sym_code, auditor), "auditor`s balance should be 0");

        auto auditor_tb = auditors(get_self(), sym_code.raw());
        auto aud_iter = auditor_tb.find(auditor.value);
        if(actn == "add") {
            eosio_assert(aud_iter == auditor_tb.end(), "auditor has been assigned based on sym_code");
            auditor_tb.emplace(get_self(), [&](auto& aud) {
                aud.auditor = auditor;
            });
        } else if(actn == "remove") {
            eosio_assert(aud_iter != auditor_tb.end(), "No auditor can be removed based on sym_code");
            auditor_tb.erase(aud_iter);
        }
    }

    void pegtoken::setgatherer( symbol_code sym_code, name gatherer) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, gatherer);
        ACCOUNT_CHECK(gatherer);
        // Check if the address is already bound
        eosio_assert(addr_check(sym_code, gatherer), "this account has assigned to address already");
        // Balance check
        eosio_assert(balance_check(sym_code, gatherer), "gatherer`s balance should be 0");

        auto gather_tb = gatherers(get_self(), sym_code.raw());
        if (gather_tb.begin() == gather_tb.end()) {
            gather_tb.emplace(get_self(), [&](auto& p) {
                p.gatherer = gatherer;
            });
        } else {
            gather_tb.erase(gather_tb.begin());
            gather_tb.emplace(get_self(), [&](auto& p) {
                p.gatherer = gatherer;
            });
        }
    }

    void pegtoken::setteller( symbol_code sym_code, name teller) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, teller);
        ACCOUNT_CHECK(teller);
        
        // Check if the address is already bound
        eosio_assert(addr_check(sym_code, teller), "this account has assigned to address already");
        // Balance check
        eosio_assert(balance_check(sym_code, teller), "teller`s balance should be 0");

        auto teller_tb = tellers(get_self(), sym_code.raw());
        if (teller_tb.begin() == teller_tb.end()) {
            teller_tb.emplace(get_self(), [&](auto& p) {
                p.teller = teller;
            });
        } else {
            teller_tb.erase(teller_tb.begin());
            teller_tb.emplace(get_self(), [&](auto& p) {
                p.teller = teller;
            });
        }
    }

    void pegtoken::setmanager( symbol_code sym_code, name manager ) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, manager);
        ACCOUNT_CHECK(manager);
        
        // Check if the address is already bound
        eosio_assert(addr_check(sym_code, manager), "this account has assigned to address already");
        // Balance check
        eosio_assert(balance_check(sym_code, manager), "manager`s balance should be 0");

        auto manager_tb = managers(get_self(), sym_code.raw());
        if (manager_tb.begin() == manager_tb.end()) {
            manager_tb.emplace(get_self(), [&](auto& mgr) {
                mgr.manager = manager;
            });
        } else {
            manager_tb.erase(manager_tb.begin());
            manager_tb.emplace(get_self(), [&](auto& mgr) {
                mgr.manager = manager;
            });
        }
    }

    void pegtoken::setbrakeman( symbol_code sym_code, name brakeman ) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, brakeman);
        ACCOUNT_CHECK(brakeman);
        
        // Check if the address is already bound
        eosio_assert(addr_check(sym_code, brakeman), "this account has assigned to address already");
        // Balance check
        eosio_assert(balance_check(sym_code, brakeman), "brakeman`s balance should be 0");

        auto brakeman_tb = brakemans(get_self(), sym_code.raw());
        if (brakeman_tb.begin() == brakeman_tb.end()) {
            brakeman_tb.emplace(get_self(), [&](auto& p) {
                p.brakeman = brakeman;
            });
        } else {
            brakeman_tb.erase(brakeman_tb.begin());
            brakeman_tb.emplace(get_self(), [&](auto& p) {
                p.brakeman = brakeman;
            });
        }
    }

    void pegtoken::setvip( symbol_code sym_code, string actn, name vip ) {
        is_auth_manager(sym_code);
        is_auth_role(sym_code, vip);
        ACCOUNT_CHECK(vip);
        
        if (actn == "add") {
            eosio_assert(!is_vip(sym_code, vip), "the account has been set to vip for the sym");
        }
        auto sym = symbol(sym_code, SYMBOL_PRECISION);
        auto vip_tb = vips(get_self(), sym_code.raw());
        auto viplimit_table = viplimits(get_self(), sym_code.raw());
        auto vipfee_table = vipfees(get_self(), sym_code.raw());
        auto vip_iter = vip_tb.find(vip.value);
        if (actn == "add") {
            if(vip_iter == vip_tb.end()) {
                vip_tb.emplace(get_self(), [&](auto& p) {
                    p.vip = vip;
                    p.create_time = time_point_sec(now());
                });
                // Init viplimit and vipfee
                viplimit_table.emplace(get_self(), [&](auto &p) {
                    p.owner = vip;
                    p.maximum_limit = eosio::asset(MAXIMUM_LIMIT, sym);
                    p.minimum_limit = eosio::asset(MINIMUM_LIMIT, sym);
                    p.total_limit = eosio::asset(TOTAL_LIMIT, sym);;
                    p.frequency_limit = FREQUENCY_LIMIT;
                    p.interval_limit = INTERVAL_LIMIT;
                });
                vipfee_table.emplace(get_self(), [&](auto &p) {
                    p.owner = vip;
                    p.service_fee_rate = SERVICE_FEE_RATE;
                    p.min_service_fee = eosio::asset(MIN_SERVICE_FEE, sym);
                    p.miner_fee = eosio::asset(MINER_FEE, sym);
                });
            } else {
                vip_tb.modify(vip_iter, same_payer, [&](auto &vp) {
                    vp.vip = vip;
                });
            }
        } else if (actn == "remove") {
            vip_tb.erase(vip_iter);
            viplimit_table.erase(viplimit_table.find(vip.value));
            vipfee_table.erase(vipfee_table.find(vip.value));
        }
    }

    void pegtoken::applyaddr( symbol_code sym_code, name to ) {
        ACCOUNT_CHECK(to);
        require_auth(to);
        is_auth_role_exc_gatherer(sym_code, to);
        // 根据sym_code，在addrs表中增加一条记录。
        // 此时owner字段置为to，address字段为空， state字段为owner.value
        // create_time为当前时间，assign_time为空
        auto addresses = addrs(get_self(), sym_code.raw());
        eosio_assert(addresses.find(to.value) == addresses.end(), "to account has applied for address already");
        addresses.emplace(get_self(), [&](auto &p) {
            p.owner = to;
            p.state = to.value;
            p.create_time = time_point_sec(now());  
            // assign_time为空
            // address字段为空
        });
    }

    void pegtoken::resetaddress( symbol_code sym_code, name to ) {
        is_auth_manager(sym_code);
        ACCOUNT_CHECK(to);
        eosio_assert(to == get_gatherer(sym_code), "not gatherer.");

        auto addrs_tb = addrs(get_self(), sym_code.raw());
        auto addr_iter = addrs_tb.find(to.value);
        eosio_assert(addr_iter != addrs_tb.end(), "account does not exist in addrs table");
        eosio_assert(addr_iter->address != "", "account address shoule not be empty");
        addrs_tb.modify(addr_iter, same_payer, [&](auto &p) {
            p.state = to.value;
            p.address = "";
        });

        auto records_tb = records(get_self(), sym_code.raw());
        auto record_iter = records_tb.find(to.value);
        if(record_iter == records_tb.end()) {
            records_tb.emplace(get_self(), [&](auto &p) {
                p.owner = to;
                p.address = "";
                p.reset_time = time_point_sec(now());
            });
        } else {
             records_tb.modify(record_iter, same_payer, [&](auto &p) {
                p.address = "";
                p.reset_time = time_point_sec(now());
             });
        }
    }

    void pegtoken::assignaddr(symbol_code sym_code, name to, string address) {
        is_auth_teller(sym_code);
        is_auth_role_exc_gatherer(sym_code, to);
        
        ACCOUNT_CHECK(to);
        auto sym_raw = sym_code.raw();
        auto infos_table = infos(get_self(), sym_raw);
        auto info_iter = infos_table.find(sym_raw);
        eosio_assert(info_iter != infos_table.end(), "token not exist");
        verify_address(info_iter->address_style, address);
        
        auto records_tb = records(get_self(), sym_raw);
        auto records_by_addr = records_tb.template get_index<"addr"_n>();
        eosio_assert(records_by_addr.find(hash64(address)) == records_by_addr.end(), "address already in use");

        auto addrs_tb = addrs(get_self(), sym_raw);
        auto addrs_by_addr = addrs_tb.template get_index<"addr"_n>();
        eosio_assert(addrs_by_addr.find(hash64(address)) == addrs_by_addr.end(), "address already in use");

        auto addr_iter = addrs_tb.find(to.value);
        eosio_assert(addr_iter != addrs_tb.end(), "addrs not exist in addrs table");
        eosio_assert(addr_iter->address == "", "address in addrs table already existed.");
        addrs_tb.modify(addr_iter, same_payer, [&](auto &p) {
            p.address = address;
            p.assign_time = time_point_sec(now());
            p.state = 0;
        });
    }

     void pegtoken::pay( asset quantity , name user){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        
        eosio_assert(quantity.amount > 0, "quantity should be more than zero for pay");
        name gatherer = get_gatherer(sym_code);
        sub_balance(user, quantity);
        add_balance(gatherer, quantity, same_payer);
     }

     void pegtoken::ruin( asset quantity, name user ) {
        require_auth(user);
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(quantity.amount > 0, "quantity should be more than zero for ruin");       
        eosio_assert(quantity>asset{0,quantity.symbol}, "The quantity to ruin is less or equal to 0");
        eosio_assert(is_locked(sym_code), "The token has been locked");
        eosio_assert(quantity.amount > 0, "The quantity to ruin is less or equal to 0");
        //如何导入账户
        // is_auth_role(sym_code);
        sub_balance(user, quantity);
        auto info_tb = infos(get_self(), sym_code.raw());
        auto info_iter2 = info_tb.get(sym_code.raw(), "sym_code do not exist in infos table");
        info_tb.modify(info_iter2, same_payer, [&](auto &p) { p.supply -= quantity; });
    }

    void pegtoken::retreat(name to, asset quantity) {
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(quantity.amount > 0, "The quantity to ruin is less or equal to 0");
        is_auth_teller(sym_code);
        add_balance(to, quantity, same_payer);
    }

    void pegtoken::confirmback( symbol_code sym_code, transaction_id_type trx_id,
        string remote_trx_id, uint64_t index, uint64_t remote_index, string memo ) {
        is_auth_teller(sym_code);
        eosio_assert(is_locked(sym_code), "The token is locked");

        auto melt_tb = melts(get_self(), sym_code.raw());
        for (auto melt_iter = melt_tb.begin(); melt_iter != melt_tb.end(); ++melt_iter) {
            // find the trx hash
            //下面的检查条件是melt需要审核且审核通过或者melt无须审核的情况
            if( std::memcmp(trx_id.hash, melt_iter->trx_id.hash, 32) == 0 && melt_iter->index == index
                && ((melt_iter->enable == true && melt_iter->need_check == true) || melt_iter->need_check == false) && melt_iter->state == 0) {
                melt_tb.modify(melt_iter, same_payer, [&](auto &mit) {
                    mit.remote_index = remote_index;
                    mit.remote_trx_id = remote_trx_id;
                    mit.state = melt_state::WITHDRAW_SUCCESS;
                    mit.msg = memo;
                });
            }
        }
    }

    void pegtoken::denyback( symbol_code sym_code, transaction_id_type trx_id,
       uint64_t index, string memo ) {
       eosio_assert(is_locked(sym_code), "The token is locked");
       is_auth_teller(sym_code);
       is_locked(sym_code);
       
        auto melt_tb = melts(get_self(), sym_code.raw());
        name melt_to;
        asset melt_total;
        asset melt_amount;
        for (auto melt_iter = melt_tb.begin(); melt_iter != melt_tb.end(); ++melt_iter) {
            // find the trx hash
            if( std::memcmp(trx_id.hash, melt_iter->trx_id.hash, 32) == 0 && melt_iter->index == index
                && ((melt_iter->enable == true && melt_iter->need_check == true) || melt_iter->need_check == false) && melt_iter->state == 0) {
                melt_to = melt_iter->from;
                melt_total = melt_iter->total;
                melt_amount = melt_iter->amount;
                melt_tb.modify(melt_iter, same_payer, [&](auto &mit) {
                    mit.state = 5;
                    mit.msg = memo;
                });
                break;
            }
        }
        auto info_tb = infos(get_self(), sym_code.raw());
        auto info_iter = info_tb.get(sym_code.raw(), "sym_code do not exist in infos table");
        info_tb.modify(info_iter, same_payer, [&](auto &p) { p.supply += melt_amount ; });

        action(
            permission_level{get_self(), "active"_n},
            get_self(),
            "retreat"_n,
            std::make_tuple(melt_to, melt_amount)
        ).send();
    }

    void pegtoken::lockall( symbol_code sym_code, name brakeman ) {
       is_auth_brakeman(sym_code);
       require_auth(brakeman);
        auto sym_raw = sym_code.raw();
        auto infos_tb = infos(get_self(), sym_raw);
        auto iter = infos_tb.find(sym_raw);
        eosio_assert(iter != infos_tb.end(), "token not exist");
        {
            auto braks = brakemans(get_self(), sym_raw);
            eosio_assert(braks.find(brakeman.value) != braks.end(), "brakeman not exist");
        }
        eosio_assert(iter->active == false, "this token has already been locked");
        infos_tb.modify(iter, same_payer, [&](auto &p) { p.active = false; });
    }

    void pegtoken::unlockall(symbol_code sym_code, name brakeman) {
       is_auth_brakeman(sym_code);
       require_auth(brakeman);
       auto sym_raw = sym_code.raw();
       auto infos_tb = infos(get_self(), sym_raw);
       auto iter = infos_tb.find(sym_raw);
       eosio_assert(iter != infos_tb.end(), "token not exist");
       {
           auto braks = brakemans(get_self(), sym_raw);
           eosio_assert(braks.find(brakeman.value) != braks.end(), "brakeman not exist");
        }
        eosio_assert(iter->active == true, "this token is not being locked");
        infos_tb.modify(iter, same_payer, [&](auto &p) { p.active = true; });
    }
} // namespace eosio

// FIXME: setauditor is removed
EOSIO_DISPATCH( eosio::pegtoken, (create)(setissuer)
        (setlimit)
        (setviplimit)
        (setfee)
        (setvipfee)
        (setcheck)
        (cast)(precast)(agreecast)(refusecast)
        (melt)
        (applyaddr)(resetaddress)(assignaddr)
        (pay)(ruin)(retreat)
        (confirmback)(denyback)
        (lockall)(unlockall)
        (setauditor)(setgatherer)(setteller)(setmanager)(setbrakeman)(setvip)
        );
