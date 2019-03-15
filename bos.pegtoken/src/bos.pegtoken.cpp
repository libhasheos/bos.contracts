#include <bos.pegtoken/bos.pegtoken.hpp>
// #include "../include/bos.pegtoken.hpp"
#include <eosiolib/transaction.hpp>
#include <eosiolib/action.hpp>

#include "bos.pegtoken_v1.cpp"
#include "bos.pegtoken_v2.cpp"

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

        create_v2(sym, issuer, address_style, peg);
    }

    void pegtoken::setissuer( symbol_code sym_code, name issuer )
    {
        setissuer_v2(sym_code, issuer);
    }

    void pegtoken::setedition( symbol_code sym_code ) {
        require_auth(get_self());
        auto edtion_table = editions(get_self(),sym_code.raw());
        eosio_assert(edtion_table.begin() == edtion_table.end(), " edition already set" );
        edtion_table.emplace(get_self(),[&](auto &p) {
            p.sym = eosio::symbol(sym_code.raw());
            p.edition = 1;
        });
    }

    // 没有版本名称，通过函数补充设置
    void pegtoken::setpeg( symbol_code sym_code ) {
        require_auth(get_self());
        auto peg_table = pegs(get_self(),sym_code.raw());
        eosio_assert(peg_table.begin() == peg_table.end(), " peg already set" );
        peg_table.emplace(get_self(),[&](auto &p) {
            p.sym = eosio::symbol(sym_code.raw());
            p.peg = 1;
        });
    }

    void pegtoken::update(symbol_code sym_code, string organization, string website) {
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2,  "Wrong Edition");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            update_v1(sym_code,organization,website);
            break;
        case 2:
            update_v2(sym_code,organization,website);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setlimit( symbol_code sym_code, asset maximum_limit, asset minimum_limit,
        asset total_limit, uint64_t frequency_limit, uint64_t interval_limit ) {
        is_auth_manager(sym_code);
        eosio_assert(is_sym_equal_asset(sym_code, maximum_limit), "sym_code is not same as maximum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, minimum_limit), "sym_code is not same as minimum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, total_limit), "sym_code is not same as total_limit symbol_code.");
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        eosio_assert(minimum_limit.amount >= 0 && maximum_limit >= minimum_limit && total_limit >= maximum_limit, "constrict mismatch: total_limit >= maximum_limit >= minimum_limit >= 0");
        setlimit_v2(sym_code, maximum_limit, minimum_limit, total_limit, frequency_limit, interval_limit);
    }

    void pegtoken::setmaxlimit( symbol_code sym_code, asset maximum_limit ) {
        eosio_assert(is_sym_equal_asset(sym_code, maximum_limit), "sym_code is not same as asset symbol_code.");
        setmaxlimit_v2(maximum_limit);
    }

    void pegtoken::setminlimit( symbol_code sym_code, asset minimum_limit ) {
        eosio_assert(is_sym_equal_asset(sym_code, minimum_limit), "sym_code is not same as asset symbol_code.");
        setminlimit_v2(minimum_limit);
    }

    void pegtoken::settotalimit( symbol_code sym_code, asset total_limit ) {
        eosio_assert(is_sym_equal_asset(sym_code, total_limit), "sym_code is not same as asset symbol_code.");
        settotalimit_v2(total_limit);
    }

    void pegtoken::setfreqlimit( symbol_code sym_code, uint64_t frequency_limit) {
        setfreqlimit_v2(sym_code, frequency_limit);
    }

    void pegtoken::setintvlimit( symbol_code sym_code, uint64_t interval_limit ) {
        setintvlimit_v2(sym_code, interval_limit);
    }

    // void pegtoken::setreslimit( symbol_code sym_code, uint64_t reset_limit) {
    //     auto peg_table = pegs(get_self(),sym_code.raw());
    //     auto pegval = peg_table.get(sym_code.raw(),"no such peg");
    //     switch (pegval.peg)
    //     {
    //     case 1:
    //         setreslimit_v1(sym_code, reset_limit);
    //         break;
    //     case 2:
    //         setreslimit_v2(sym_code, reset_limit);
    //         break;
    //     default:
    //         eosio_assert(false, "edition should be either 1 or 2");
    //         break;
    //     }
    // }

    void pegtoken::setviplimit( symbol_code sym_code, name vip, asset maximum_limit,asset minimum_limit,
        asset total_limit, uint64_t frequency_limit, uint64_t interval_limit ) {
        is_auth_manager(sym_code);
        eosio_assert(is_sym_equal_asset(sym_code, maximum_limit), "sym_code is not same as maximum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, minimum_limit), "sym_code is not same as minimum_limit symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, total_limit), "sym_code is not same as total_limit symbol_code.");
        setviplimit_v2(vip, maximum_limit, minimum_limit, total_limit, frequency_limit, interval_limit);
    }

    void pegtoken::setvipmaxlim(name vip, asset maximum_limit ) {
        auto sym_code = maximum_limit.symbol.code();
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2,   "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipmaxlim_v1(vip,maximum_limit );
            break;
        case 2:
            setvipmaxlim_v2(vip,maximum_limit );
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipminlim(name vip, asset minimum_limit) {
        auto sym_code = minimum_limit.symbol.code();
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2, "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipminlim_v1(vip,minimum_limit );
            break;
        case 2:
            setvipminlim_v2(vip,minimum_limit );
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setviptotlim(name vip, asset totimum_limit ) {
        auto sym_code = totimum_limit.symbol.code();
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2,   "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setviptotlim_v1(vip,totimum_limit );
            break;
        case 2:
            setviptotlim_v2(vip,totimum_limit );
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipfreqlm(symbol_code sym_code, name vip, uint64_t frequency_limit) {
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2, "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            // setvipfreqlm_v1(sym_code, vip, frequency_limit);
            break;
        case 2:
            setvipfreqlm_v2(sym_code, vip, frequency_limit);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipintvlm(symbol_code sym_code, name vip, uint64_t interval_limit) {
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2, "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            // setvipintvlm_v1(sym_code, vip, interval_limit);
            break;
        case 2:
            setvipintvlm_v2(sym_code, vip, interval_limit);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setfee( symbol_code sym_code, double service_fee_rate,
        asset min_service_fee, asset miner_fee ) {
        eosio_assert(is_sym_equal_asset(sym_code, min_service_fee), "sym_code is not same as min_service_fee symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, miner_fee), "sym_code is not same as miner_fee symbol_code.");
        is_auth_manager(sym_code);
        setfee_v2(sym_code, service_fee_rate, min_service_fee, miner_fee);
    }

    void pegtoken::setservfeert(symbol_code sym_code, double service_fee_rate) {
        eosio_assert(getedition(sym_code) != 1 && getedition(sym_code) != 2,   "Edition should be either 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setservfeert_v1(sym_code,service_fee_rate);
            break;
        case 2:
            setservfeert_v2(sym_code,service_fee_rate);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setminserfee(asset min_service_fee) {
        auto sym_code = min_service_fee.symbol.code();
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setminserfee_v1(min_service_fee);
            break;
        case 2:
            setminserfee_v2(min_service_fee);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setminerfee(asset miner_fee) {
        auto sym_code = miner_fee.symbol.code();
        auto editionval = getedition(sym_code);
        switch (editionval){
            case 1:
                setminerfee_v1(miner_fee);
                break;
            case 2:
                setminerfee_v2(miner_fee);
                break;
            default:
                eosio_assert(false, "edition should be either 1 or 2");
                break;
        }
    }

    void pegtoken::setvipfee(symbol_code sym_code, name vip, double service_fee_rate, asset min_service_fee, asset miner_fee ){
        is_auth_manager(sym_code);
           //  判断所有的asset 是否与sym_code为同一种币，若不是，则报错
        eosio_assert(is_sym_equal_asset(sym_code, min_service_fee), "sym_code is not same as min_service_fee symbol_code.");
        eosio_assert(is_sym_equal_asset(sym_code, miner_fee), "sym_code is not same as miner_fee symbol_code.");
        // 根据sym_code，查询editions表，校验币的版本，版本不对则报错。
        // 根据sym_code，查询pegs表，校验币的机制，机制不对则报错。
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        eosio_assert(getpeg(sym_code) == 1 || getpeg(sym_code) == 2, "The action require peg to be 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipfee_v1(vip,service_fee_rate, min_service_fee, miner_fee);
            break;
        case 2:
            setvipfee_v2(vip,service_fee_rate, min_service_fee, miner_fee);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipserfrt(symbol_code sym_code, name vip,double service_fee_rate){
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipserfrt_v1(sym_code,vip,service_fee_rate);
            break;
        case 2:
            setvipserfrt_v2(sym_code,vip,service_fee_rate);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipminfee(name vip, asset min_service_fee ) {
        auto sym_code = min_service_fee.symbol.code();
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipminfee_v1(vip,min_service_fee);
            break;
        case 2:
            setvipminfee_v2(vip,min_service_fee);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setvipminerf(name vip, asset miner_fee ) {
        auto sym_code = miner_fee.symbol.code();
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setvipminerf_v1(vip,miner_fee);
            break;
        case 2:
            setvipminerf(vip,miner_fee);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setdelay(symbol_code sym_code, uint64_t delayday) {
        // 根据sym_code，查询editions表，校验币的版本，版本不对则报错。
        // 根据sym_code，查询pegs表，校验币的机制，机制不对则报错。
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        eosio_assert(getpeg(sym_code) == 1 || getpeg(sym_code) == 2, "The action require peg to be 1 or 2");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            setdelay_v1(sym_code,delayday);
            break;
        case 2:
            setdelay_v2(sym_code,delayday);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::setcheck( symbol_code sym_code, bool in_check, bool out_check ){
        is_auth_manager(sym_code);
        // 根据sym_code，查询editions表，校验币的版本，版本不对则报错。
        // 根据sym_code，查询pegs表，校验币的机制，机制不对则报错。
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        eosio_assert(getpeg(sym_code) == 1 || getpeg(sym_code) == 2, "The action require peg to be 1 or 2");
        setcheck_v2(sym_code, in_check, out_check);
    }


    void pegtoken::setincheck( symbol_code sym_code, bool in_check){
            // setincheck_v1( sym_code, in_check);
            // setincheck_v2( symbol_code sym_code, in_check);
    }


    void pegtoken::setoutcheck( symbol_code sym_code, bool out_check){
    //     setoutcheck_v1(  sym_code,  out_check);
    //     setoutcheck_v2(  sym_code,  out_check);

    }




    void pegtoken::issue(asset quantity, string memo) {
        auto sym_code = quantity.symbol.code();
        is_auth_issuer(sym_code);
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        STRING_LEN_CHECK(memo, 256)
        eosio_assert(quantity.is_valid() && quantity.amount > 0, "invalid quantity");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            issue_v1(quantity,memo);
            break;
        case 2:
            issue_v2(quantity,memo);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::retire(asset quantity, string memo) {
        eosio_assert(quantity.is_valid() && quantity.amount > 0, "invalid quantity");
        STRING_LEN_CHECK(memo, 256)
        auto sym_code = quantity.symbol.code();
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            retire_v1(quantity,memo);
            break;
        case 2:
            retire_v2(quantity,memo);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::precast(symbol_code sym_code, string to_address, name to_account, 
        string remote_trx_id, asset quantity, uint64_t index, string memo) {
        is_auth_teller(sym_code);
        is_auth_role(sym_code, to_account);
        //  判断所有的 asset 是否与 sym_code 为同一种币，若不是，则报错
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        // 判断检查是否已经锁定
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        ACCOUNT_CHECK(to_account)
        STRING_LEN_CHECK(memo, 256)
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        precast_v2(sym_code, to_address, to_account, remote_trx_id, quantity, index,memo);
    }

    void pegtoken::agreecast(symbol_code sym_code, string to_address, name to_account,
        string remote_trx_id, asset quantity, uint64_t index, string memo) {
        // 判断 to_account 不能是 issuer、auditor 等角色账户
        is_auth_auditor(sym_code);
        is_auth_role(sym_code, to_account);
        // 判断所有的 asset 是否与 sym_code 为同一种币，若不是，则报错
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        // 判断是否已经锁定
        eosio_assert(is_locked(sym_code), "The token is locked");
        // 判断资金流入是否需要审核
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        // 币种只能为严格锚定模式才能调用
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        ACCOUNT_CHECK(to_account)
        STRING_LEN_CHECK(memo, 256)
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        agreecast_v2(sym_code, to_address, to_account, remote_trx_id, quantity, index, memo);
    }

    void pegtoken::refusecast(symbol_code sym_code, string to_address, name to_account,
        string remote_trx_id, asset quantity, uint64_t index, string memo) {
        is_auth_auditor(sym_code);
        is_auth_role(sym_code, to_account);
        // 判断所有的 asset 是否与 sym_code 为同一种币，若不是，则报错
        eosio_assert(is_sym_equal_asset(sym_code, quantity), "sym_code is not same as quantity's symbol_code.");
        // 判断是否已经锁定
        eosio_assert(is_locked(sym_code), "The token is locked");
        // 判断资金流入是否需要审核
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        // 币种只能为严格锚定模式才能调用
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        ACCOUNT_CHECK(to_account)
        STRING_LEN_CHECK(memo, 256)
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        refusecast_v2(sym_code, to_address, to_account, remote_trx_id, quantity, index, memo);
    }

    void pegtoken::cast(symbol_code sym_code, string to_address, name to_account, string remote_trx_id, asset quantity,  uint64_t index, string memo){
        is_auth_role(sym_code,to_account);
        // 判断是否已经锁定
        eosio_assert(is_locked(sym_code),"The token has been locked");
        // 判断资金流入是否需要审核
        eosio_assert(!getincheck(sym_code), "This action require in_check to be false");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        ACCOUNT_CHECK(to_account)
        STRING_LEN_CHECK(memo,256)
        eosio_assert(quantity.amount > 0, "non-positive quantity");
        docast_v2(to_address,to_account,remote_trx_id,index,quantity,memo);
    }
    // 用户转给出纳员
    void pegtoken::melt(name from_account, string to_address, asset quantity, uint64_t index, string memo){
        symbol_code sym_code = quantity.symbol.code();
        withdraw_check(sym_code, quantity, from_account);
        //不能为角色账户，但除去gatherer
        is_auth_role_exc_gatherer(sym_code,from_account);
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(!getoutcheck(sym_code), "This action require out_check to be false");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        melt_v2(from_account, to_address, quantity,  index,  memo);
    }

    void pegtoken::premelt(name from_account, string to_address, asset quantity, uint64_t index, string memo){
        symbol_code sym_code = quantity.symbol.code();
        withdraw_check(sym_code, quantity, from_account);
        eosio_assert(is_locked(sym_code),"The token has been locked");
        // eosio_assert(getoutcheck(sym_code), "This action require out_check to be true");
        eosio_assert(!getoutcheck(sym_code), "This action require out_check to be false");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
    }

    void pegtoken::agreemelt(name from_account, string to_address, asset quantity, uint64_t index, string memo){
        symbol_code sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getoutcheck(sym_code), "This action require out_check to be true");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        // eosio_assert(!getpeg());
    }

    void pegtoken::refusemelt(name from_account, string to_address, asset quantity, uint64_t index, string memo){
        symbol_code sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getoutcheck(sym_code), "This action require out_check to be true");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        refusemelt_v2(from_account, to_address, quantity, index, memo);
        // eosio_assert(!getpeg());
    }

    //TODO:
    void pegtoken::rm( symbol_code sym_code, uint64_t id, uint64_t type){ 
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");     
    }

    // check 5 roles: deployer, teller, gatherer, manager, brakeman, issuer (check in different version)
    void pegtoken::setauditor( symbol_code sym_code, string actn, name auditor ) {
        is_auth_issuer(sym_code);
        if (actn == "add") {
            is_auth_role(sym_code, auditor);
        }
        ACCOUNT_CHECK(auditor);
        setauditor_v2(sym_code, actn, auditor);
    }

    void pegtoken::setgatherer( symbol_code sym_code, name gatherer) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, gatherer);
        ACCOUNT_CHECK(gatherer);
        eosio_assert(getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be strict anchor.");
        setgatherer_v2(sym_code, gatherer);
    }

    void pegtoken::setteller( symbol_code sym_code, name teller) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, teller);
        ACCOUNT_CHECK(teller);
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        setteller_v2(sym_code, teller);
    }

    void pegtoken::setmanager( symbol_code sym_code, name manager ) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, manager);
        ACCOUNT_CHECK(manager);
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        setmanager_v2(sym_code, manager);
    }

    void pegtoken::setbrakeman( symbol_code sym_code, name brakeman ) {
        is_auth_issuer(sym_code);
        is_auth_role(sym_code, brakeman);
        ACCOUNT_CHECK(brakeman);
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        setbrakeman_v2(sym_code, brakeman);
    }

    void pegtoken::setvip( symbol_code sym_code, string actn, name vip ) {
        is_auth_manager(sym_code);
        is_auth_role(sym_code, vip);
        ACCOUNT_CHECK(vip);
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        if (actn == "add") {
            eosio_assert(!is_vip(sym_code, vip), "the account has been set to vip for the sym");
        }
        setvip_v2(sym_code, actn, vip);
    }

    void pegtoken::applyaddr(symbol_code sym_code, name to) {
        // 根据sym_code，查询editions表，校验币的版本，版本不对则报错。
        // 根据sym_code，查询pegs表，校验币的机制，机制不对则报错。
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        eosio_assert(getpeg(sym_code) == 1 || getpeg(sym_code) == 2, "The action require peg to be 1 or 2");
        ACCOUNT_CHECK(to);
        is_auth_role(sym_code, to);
        auto editionval = getedition(sym_code);
        switch (editionval){
            case 1:
                applyaddr_v1(sym_code,to);
                break;
            case 2:
                applyaddr_v2(sym_code,to);
            default:
                eosio_assert(false, "edition should be either 1 or 2");
                break;
        }
    }
    

    void pegtoken::resetaddress( symbol_code sym_code, name to ){
        is_auth_manager(sym_code);
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        eosio_assert(getpeg(sym_code) == 1 || getpeg(sym_code) == 2, "The action require peg to be 1 or 2");
        ACCOUNT_CHECK(to);
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            resetaddress_v1(sym_code,to);
            break;
        case 2: 
            resetaddress_v2(sym_code,to);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::assignaddr(symbol_code sym_code, name to, string address) {
        is_auth_teller(sym_code);
        is_auth_role(sym_code, to);
        eosio_assert(getpeg(sym_code) == peg_type::PRE_RELEASE || getpeg(sym_code) == peg_type::STRICT_ANCHOR, "The action require peg to be pre release or strict anchor");
        ACCOUNT_CHECK(to);
        assignaddr_v2(sym_code, to, address);
    }

    void pegtoken::withdraw(name from, string to, asset quantity, uint64_t index, string memo) {
        auto sym_code = quantity.symbol.code();
        withdraw_check(sym_code, quantity, from);
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(!getoutcheck(sym_code), "This action require out_check to be false");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            withdraw_v1(from,to,quantity,memo);
            break;
        case 2:
            withdraw_v2(from,to,quantity, index, memo);
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }


    void pegtoken::prewithdraw( name from, string to, asset quantity, uint64_t index, string memo){
        auto sym_code = quantity.symbol.code();
        withdraw_check(sym_code, quantity, from);
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(!getoutcheck(sym_code), "out_check is true for this symbol");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
    }

    void pegtoken::agrewithdraw( name from, string to, asset quantity, uint64_t index, string memo){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(!getoutcheck(sym_code), "out_check is true for this symbol");
    }

    void pegtoken::refuwithdraw( name from, string to, asset quantity, uint64_t index, string memo){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(!getoutcheck(sym_code), "out_check is true for this symbol");
    }


    void pegtoken::deposit(string from, name to, asset quantity, string remote_trx_id, uint64_t index, string memo) {
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(!getincheck(sym_code), "This action require in_check to be false");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        deposit_v1(to,quantity,memo);
    }


    void pegtoken::predeposit( string from, name to, asset quantity, string remote_trx_id, uint64_t index, string memo ){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
    }


    void pegtoken::agreedeposit( string from, name to, asset quantity, string remote_trx_id, uint64_t index, string memo ){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
    }

    void pegtoken::refusdeposit( string from, name to, asset quantity, string remote_trx_id, uint64_t index, string memo ){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getincheck(sym_code), "This action require in_check to be true");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
    }

    void pegtoken::transfer(name from, name to, asset quantity, string memo) {
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            transfer_v1(from,to,quantity,memo);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }
    void pegtoken::remit( name from, asset quantity ){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
    }

     void pegtoken::pay( asset quantity ){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
     }
    
    // TODO: finish ruin_v2
     void pegtoken::ruin( asset quantity, name user ){
        require_auth(user);
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        eosio_assert(quantity>asset{0,quantity.symbol}, "The quantity to ruin is less or equal to 0");
        //如何导入账户
        // is_auth_role(sym_code);
        ruin_v2(quantity, user);
    }

    void pegtoken::burn( name from, asset quantity){
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");

    }

    void pegtoken::retreat(name to, asset quantity) {
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code), "The token is locked");
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "This action require peg version to be 2.");
        eosio_assert(quantity.amount > 0, "The quantity to ruin is less or equal to 0");
        is_auth_teller(sym_code);
        retreat_v2(to, quantity);
    }

    void pegtoken::clear(symbol_code sym_code, uint64_t num) {
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        clear_v1(sym_code,num);
    }

    void pegtoken::pubminerfee( asset miner_fee ){
        auto sym_code = miner_fee.symbol.code();
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
    }

    void pegtoken::publishsum( string address, asset sum ){
        auto sym_code = sum.symbol.code();
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
    }


    void pegtoken::feedback(symbol_code sym_code, transaction_id_type trx_id, string remote_trx_id, string memo) {
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        feedback_v1(sym_code,trx_id,remote_trx_id,memo);
    }

    void pegtoken::rollback(symbol_code sym_code, transaction_id_type trx_id, string memo) {
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        eosio_assert(getedition(sym_code) == 1 || getedition(sym_code) == 2, "The action require edition to be 1 or 2");
        rollback_v1(sym_code,trx_id,memo);
    }

    void pegtoken::confirmback( symbol_code sym_code, transaction_id_type trx_id,
        string remote_trx_id, uint64_t index, uint64_t remote_index, string memo ) {
        eosio_assert(is_locked(sym_code), "The token is locked");
        is_auth_teller(sym_code);
        eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
        eosio_assert(getpeg(sym_code) == 2, "The action require peg to be 2");
        confirmback_v2(sym_code, trx_id, remote_trx_id, index, remote_index, memo);
    }

    void pegtoken::denyback( symbol_code sym_code, transaction_id_type trx_id,
       uint64_t index, string memo ) {
       eosio_assert(is_locked(sym_code), "The token is locked");
       is_auth_teller(sym_code);
       eosio_assert(getedition(sym_code) == 2, "The action require edition to be 2");
       eosio_assert(getpeg(sym_code) == 2, "The action require peg to be 2");
       denyback_v2(sym_code, trx_id, index, memo);
    }



    // void pegtoken::setacceptor(symbol_code sym_code, name acceptor) {
    //     auto peg_table = pegs(get_self(),sym_code.raw());
    //     auto pegval = peg_table.get(sym_code.raw(),"no such peg");
    //     switch (pegval.peg)
    //     {
    //     case 1:
    //         setacceptor_v1(sym_code,acceptor);
    //         break;
    //     default:
    //         eosio_assert(false, "edition should be either 1 or 2");
    //         break;
    //     }
    // }


    void pegtoken::lockall(symbol_code sym_code, name brakeman) {
        // 仅brakeman有权限调用
       is_auth_brakeman(sym_code);
       auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            lockall_v1(sym_code,brakeman);
            break;
        case 2:
            lockall_v2(sym_code,brakeman);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }

    void pegtoken::unlockall(symbol_code sym_code, name brakeman) {
       // 仅brakeman有权限调用
       is_auth_brakeman(sym_code);
        auto editionval = getedition(sym_code);
        switch (editionval)
        {
        case 1:
            unlockall_v1(sym_code,brakeman);
            break;
        case 2:
            unlockall_v2(sym_code,brakeman);
            break;
        default:
            eosio_assert(false, "edition should be either 1 or 2");
            break;
        }
    }



    void pegtoken::sendback(/*name auditor,*/ transaction_id_type trx_id, name to, asset quantity, string memo) {
        auto sym_code = quantity.symbol.code();
        eosio_assert(is_locked(sym_code),"The token has been locked");
        eosio_assert(getpeg(sym_code) == 1, "This action require peg version to be 1.");
        sendback_v1(trx_id,to,quantity,memo);
    }


    // void pegtoken::approve(symbol_code sym_code, name auditor, transaction_id_type trx_id, string memo) {
    //     auto peg_table = pegs(get_self(),sym_code.raw());
    //     auto pegval = peg_table.get(sym_code.raw(),"no such peg");
    //     switch (pegval.peg)
    //     {
    //     case 1:
    //         approve_v1(sym_code,auditor,trx_id,memo);
    //         break;
    //     default:
    //         eosio_assert(false, "edition should be either 1 or 2");
    //         break;
    //     }
    // }

    // void pegtoken::unapprove(symbol_code sym_code, name auditor, transaction_id_type trx_id, string memo) {
    //     auto peg_table = pegs(get_self(),sym_code.raw());
    //     auto pegval = peg_table.get(sym_code.raw(),"no such peg");
    //     switch (pegval.peg)
    //     {
    //     case 1:
    //         unapprove_v1(sym_code,auditor,trx_id,memo);
    //         break;
    //     default:
    //         eosio_assert(false, "edition should be either 1 or 2");
    //         break;
    //     }
    // }

    // void pegtoken::rmwithdraw(uint64_t id, symbol_code sym_code) {
    //     auto peg_table = pegs(get_self(),sym_code.raw());
    //     auto pegval = peg_table.get(sym_code.raw(),"no such peg");
    //     switch (pegval.peg)
    //     {
    //     case 1:
    //         rmwithdraw_v1(id,sym_code);
    //         break;
    //     default:
    //         eosio_assert(false, "edition should be either 1 or 2");
    //         break;
    //     }
    // }
    

} // namespace eosio

// FIXME: setauditor is removed
EOSIO_DISPATCH( eosio::pegtoken, (create)(setissuer)(setedition)(setpeg)(update)
        (setlimit)(setmaxlimit)(setminlimit)(settotalimit)(setfreqlimit)(setintvlimit)
        (setviplimit)(setvipmaxlim)(setvipminlim)(setviptotlim)(setvipfreqlm)(setvipintvlm)
        (setfee)(setservfeert)(setminserfee)(setminerfee)
        (setvipfee)(setvipserfrt)(setvipminfee)(setvipminerf)
        (setdelay)(setcheck)(setoutcheck)(setincheck)
        (issue)(retire)
        (cast)(precast)(agreecast)(refusecast)
        (melt)(premelt)(agreemelt)(refusemelt)
        (applyaddr)(resetaddress)(assignaddr)
        (withdraw)(prewithdraw)(agrewithdraw)(refuwithdraw)
        (deposit)(predeposit)(agreedeposit)(refusdeposit)
        (transfer)(remit)(pay)(ruin)(burn)(retreat)
        (feedback)(rollback)(confirmback)(denyback)(sendback)
        (lockall)(unlockall)(clear)(rm)
        (setauditor)(setgatherer)(setteller)(setmanager)(setbrakeman)(setvip)
        (pubminerfee)(publishsum)
        );
