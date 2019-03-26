#pragma once

#include "decoder.hpp"
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>
#include <string>
#include <vector>

namespace eosio {

using std::string;
using std::vector;

using transaction_id_type = capi_checksum256;

constexpr uint32_t ONE_DAY = 24 * 60 * 60;
constexpr uint64_t PRECISION = 100000000;
constexpr uint8_t SYMBOL_PRECISION = 8;
constexpr uint64_t MAXIMUM_LIMIT = 1 * PRECISION;
constexpr uint64_t MINIMUM_LIMIT = 0.00005 * PRECISION;
constexpr uint64_t TOTAL_LIMIT = 10 * PRECISION;
constexpr uint64_t FREQUENCY_LIMIT = 3;
constexpr uint64_t INTERVAL_LIMIT = 300;
constexpr double SERVICE_FEE_RATE = 0.001;
constexpr uint64_t MIN_SERVICE_FEE = 0.0005 * PRECISION;
constexpr uint64_t MINER_FEE = 0.00004 * PRECISION;
const microseconds DAY_IN_MICROSECOND = microseconds(86400000000);

enum withdraw_state : uint64_t {
    INITIAL_STATE = 0,
    FEED_BACK = 2,
    SEND_BACK = 3,
    ROLL_BACK = 5,
};

enum melt_state: uint64_t {
    WITHDRAW_INIT = 0,
    WITHDRAW_SUCCESS = 2,
    WITHDRAW_BACK = 3,
    WITHDRAW_ROLLBACL = 5,
};

enum cast_state: uint64_t {
    CAST_INIT = 0,
    CAST_SUCCESS = 2,
    CAST_FAIL = 5,
};

class[[eosio::contract("bos.pegtoken")]] pegtoken : public contract
{
public:
    using contract::contract;

    [[eosio::action]] void create( symbol sym, name issuer, name address_style );

    [[eosio::action]] void setissuer( symbol_code sym_code, name issuer );
    
    [[eosio::action]] void setlimit( symbol_code sym_code, asset maximum_limit, asset minimum_limit, asset total_limit, uint64_t frequency_limit, uint64_t interval_limit );

    [[eosio::action]] void setviplimit( symbol_code sym_code, name vip, asset maximum_limit, asset minimum_limit, asset total_limit, uint64_t frequency_limit, uint64_t interval_limit );

    [[eosio::action]] void setfee( symbol_code sym_code, double service_fee_rate, asset min_service_fee, asset miner_fee );

    [[eosio::action]] void setvipfee( symbol_code sym_code, name vip, double service_fee_rate, asset min_service_fee, asset miner_fee );

    [[eosio::action]] void setcheck( symbol_code sym_code, bool in_check, bool out_check );

    [[eosio::action]] void precast( symbol_code sym_code, string to_address, name to_account, string remote_trx_id, asset quantity, uint64_t index, string memo );

    [[eosio::action]] void agreecast( symbol_code sym_code, string to_address, name to_account, name auditor, string remote_trx_id, asset quantity, uint64_t index, string memo );
    
    [[eosio::action]] void refusecast( symbol_code sym_code, string to_address, name to_account, name auditor, string remote_trx_id, asset quantity, uint64_t index, string memo );
    
    [[eosio::action]] void melt( name from_account, string to_address, asset quantity, string memo );

    [[eosio::action]] void premelt( name from_account, string to_address, asset quantity, uint64_t index, string memo );

    [[eosio::action]] void applyaddr( symbol_code sym_code, name to );

    [[eosio::action]] void resetaddress( symbol_code sym_code, name to );

    [[eosio::action]] void assignaddr( symbol_code sym_code, name to, string address );

    [[eosio::action]] void notifymelt( uint64_t id );

    [[eosio::action]] void pay( asset quantity, name user );

    [[eosio::action]] void ruin( asset quantity , name user );

    [[eosio::action]] void retreat( name to, asset quantity );

    [[eosio::action]] void confirmback( symbol_code sym_code, string remote_trx_id, uint64_t id, uint64_t remote_index, string memo );

    [[eosio::action]] void denyback( symbol_code sym_code, uint64_t id, string memo );

    [[eosio::action]] void lockall( symbol_code sym_code, name brakeman );

    [[eosio::action]] void unlockall( symbol_code sym_code, name brakeman );
    
    [[eosio::action]] void setauditor( symbol_code sym_code, string actn, name auditor );

    [[eosio::action]] void setgatherer( symbol_code sym_code, name gatherer );

    [[eosio::action]] void setteller( symbol_code sym_code, name teller );

    [[eosio::action]] void setmanager( symbol_code sym_code, name manager );

    [[eosio::action]] void setbrakeman( symbol_code sym_code, name brakeman );

    [[eosio::action]] void setvip( symbol_code sym_code, string actn, name vip );

private:
    bool getincheck( symbol_code sym_code );
    bool getoutcheck( symbol_code sym_code );
    void verify_address( name style, string address );
    void verify_txid( name style, string txid ); //TODO
    void add_balance( name owner, asset value, name ram_payer );
    void sub_balance( name owner, asset value );
    bool is_sym_equal_asset( symbol_code sym_code, asset quantity );
    bool is_vip( symbol_code sym_code, name name );
    bool is_locked( symbol_code sym_code );
    void is_auth_issuer( symbol_code sym_code );
    void is_auth_manager( symbol_code sym_code );
    void is_auth_teller( symbol_code sym_code );
    void is_auth_auditor( symbol_code sym_code, name auditor );
    void is_auth_brakeman( symbol_code sym_code );
    void is_auth_gatherer( symbol_code sym_code );
    void is_auth_role( symbol_code sym_code, name account );
    void is_auth_role_exc_gatherer( symbol_code sym_code, name account );

    name get_gatherer( symbol_code sym_code );
    name get_teller( symbol_code sym_code );

    void withdraw_check( symbol_code sym_code, asset quantity, name account );
    void vip_withdraw_check( symbol_code sym_code, asset quantity, name account );
    bool balance_check( symbol_code sym_code, name user );
    bool addr_check( symbol_code sym_code, name user );
    
    asset getbalance( symbol_code sym_code, name user );
    asset calculate_service_fee( asset sum, double service_fee_rate, asset min_service_fee );

    struct [[eosio::table]] symbol_ts {
        symbol sym;

        uint64_t primary_key() const { return sym.code().raw(); }
    };

    struct [[eosio::table]] check_ts {
        symbol sym;
        bool in_check;  // Whether the capital inflow relative to the BOS chain needs to be checked
        bool out_check; // Whether the outflow of funds needs to be checked

        uint64_t primary_key() const { return sym.code().raw(); }
    };

    struct [[eosio::table]] addr_ts {
        name owner;
        string address;
        uint64_t state;

        time_point_sec assign_time;
        time_point_sec create_time;

        uint64_t primary_key() const { return owner.value; }
        uint64_t by_addr() const { return hash64( address ); }
        uint64_t by_state() const { return state; }
    };

    struct [[eosio::table]] record_ts {
        name owner;
        string address;
        time_point_sec reset_time;

        uint64_t primary_key() const { return owner.value; }
        uint64_t by_addr() const { return hash64(address); }
    };

    struct [[eosio::table]] statistic_ts {
        name owner;
        time_point_sec last_time;
        uint64_t frequency;
        asset total;
        time_point_sec update_time;

        uint64_t primary_key() const { return owner.value; }
    };

    struct [[eosio::table]] account_ts {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    struct [[eosio::table]] melt_ts {
        uint64_t id;
        name from;
        string to;
        asset total;
        asset amount;
        asset fee;
        uint64_t state;
        bool need_check;
        bool enable;
        name auditor;
        string remote_trx_id;
        uint64_t remote_index;
        string msg;
        time_point_sec create_time;
        time_point_sec update_time;
        
        uint64_t primary_key() const { return id; }
    };
    
    struct [[eosio::table]] cast_ts {
        uint64_t id;
        transaction_id_type trx_id;
        name to_account;
        string to_address;
        asset quantity;
        uint64_t state;
        bool need_check;
        bool enable;
        name auditor;
        uint64_t index;
        string remote_trx_id;
        uint64_t remote_index;

        string msg;
        time_point_sec create_time;
        time_point_sec update_time;
        
        uint64_t primary_key() const { return id; }
        uint64_t by_state() const { return state; }
    };

    struct [[eosio::table]] vip_ts {
        name vip;
        time_point_sec create_time;

        uint64_t primary_key() const { return vip.value; }
    };

    struct [[eosio::table]] viplimit_ts {
        name owner;
        asset maximum_limit;
        asset minimum_limit;
        asset total_limit;
        uint64_t frequency_limit;
        uint64_t interval_limit; // Two tokens exchange intervals
        uint64_t reset_limit;    // Limit of reset the binding address
    
        uint64_t primary_key() const { return owner.value; }
    };

    struct [[eosio::table]] vipfee_ts {
        name owner;
        double service_fee_rate;
        asset min_service_fee;
        asset miner_fee;

        uint64_t primary_key() const { return owner.value; }
    };

    struct [[eosio::table]] info_ts {
        asset supply;
        name issuer;
        name address_style;
        bool active;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };

    struct [[eosio::table]] limit_ts {
        asset maximum_limit;
        asset minimum_limit;
        asset total_limit;
        uint64_t frequency_limit;
        uint64_t interval_limit;
        uint64_t reset_limit;

        uint64_t primary_key() const { return maximum_limit.symbol.code().raw(); }
    };

    struct [[eosio::table]] fee_ts {
        double service_fee_rate;
        asset min_service_fee;
        asset miner_fee;

        uint64_t primary_key() const { return min_service_fee.symbol.code().raw(); }
    };

    struct [[eosio::table]] manager_ts {
        name manager;

        uint64_t primary_key() const { return manager.value; }
    };

    struct [[eosio::table]] teller_ts {
        name teller;

        uint64_t primary_key() const { return teller.value; }
    };

    struct [[eosio::table]] auditor_ts {
        name auditor;

        uint64_t primary_key() const { return auditor.value; }
    };

    struct [[eosio::table]] gatherer_ts {
        name gatherer;

        uint64_t primary_key() const { return gatherer.value; }
    };

    struct [[eosio::table]] brakeman_ts {
        name brakeman;

        uint64_t primary_key() const { return brakeman.value; }
    };

    using symbols = eosio::multi_index< "symbols"_n, symbol_ts >;
    using checks = eosio::multi_index< "checks"_n, check_ts >;
    using addrs = eosio::multi_index< "addrs"_n, addr_ts,
        indexed_by< "addr"_n, const_mem_fun< addr_ts, uint64_t, &addr_ts::by_addr > >,
        indexed_by< "state"_n, const_mem_fun< addr_ts, uint64_t, &addr_ts::by_state > > >;
    using records = eosio::multi_index< "records"_n, record_ts,
        indexed_by< "addr"_n, const_mem_fun< record_ts, uint64_t, &record_ts::by_addr > > >;
    using statistics = eosio::multi_index< "statistics"_n, statistic_ts >;
    using accounts = eosio::multi_index< "accounts"_n, account_ts >;
    using melts = eosio::multi_index< "melts"_n, melt_ts >;
    using casts = eosio::multi_index< "casts"_n, cast_ts,
        indexed_by< "state"_n, const_mem_fun< cast_ts, uint64_t, &cast_ts::by_state > > >;
    using vips = eosio::multi_index< "vips"_n, vip_ts >;
    using viplimits = eosio::multi_index< "viplimits"_n, viplimit_ts >;
    using vipfees = eosio::multi_index< "vipfees"_n, vipfee_ts >;
    using infos = eosio::multi_index< "infos"_n, info_ts >;
    using limits = eosio::multi_index< "limits"_n, limit_ts >;
    using fees = eosio::multi_index< "fees"_n, fee_ts >;
    using managers = eosio::multi_index< "managers"_n, manager_ts >;
    using tellers = eosio::multi_index< "tellers"_n, teller_ts >;
    using auditors = eosio::multi_index< "auditors"_n, auditor_ts >;
    using gatherers = eosio::multi_index< "gathers"_n, gatherer_ts >;
    using brakemans = eosio::multi_index< "brakemans"_n, brakeman_ts >;
};


////////////////////////
// private funcs
////////////////////////
    void pegtoken::verify_address( name style, string addr ) {
        if (style == "bitcoin"_n) {
            eosio_assert(valid_bitcoin_addr(addr), "invalid bitcoin addr");
        } else if (style == "ethereum"_n) {
            eosio_assert(valid_ethereum_addr(addr), "invalid ethereum addr");
        } else if (style == "tether"_n) {
            auto _ = name(addr);
        } else if (style == "usdt"_n){
            eosio_assert(valid_usdt_addr(addr), "invalid USDT addr");
        } 
        else if (style == "other"_n) {
            // no check
        } else {
            eosio_assert(false,
                         "only USDT, BTC and ETH supported. address style must be one of bitcoin, ethereum, tether or other");
        }
    }

    void pegtoken::sub_balance( name owner, asset value ) {
        auto acct = accounts( get_self(), owner.value );
        auto from = acct.find( value.symbol.code().raw() );
        eosio_assert( from != acct.end(), "no balance object found" );
        eosio_assert( from->balance.amount >= value.amount, "overdrawn balance" );
        if ( from->balance.amount == value.amount ) {
            acct.erase( from );
        } else {
            acct.modify( from, same_payer, [&]( auto& p ) {
                p.balance -= value;
            } );
        }
    }

    void pegtoken::add_balance( name owner, asset value, name ram_payer ) {
        auto acct = accounts(get_self(), owner.value);
        auto to = acct.find(value.symbol.code().raw());
        if (to == acct.end()) {
            acct.emplace(ram_payer, [&](auto &p) {
                p.balance = value;
            });
        } else {
            acct.modify(to, same_payer, [&](auto &p) {
                p.balance += value;
            });
        }
    }

    asset pegtoken::calculate_service_fee( asset sum, double service_fee_rate, asset min_service_fee ) {
        asset actual_service_fee = sum * service_fee_rate;

        if (actual_service_fee.amount < min_service_fee.amount) {
            return min_service_fee;
        } else {
            return actual_service_fee;
        }
    }

    asset pegtoken::getbalance( symbol_code sym_code, name user ) {
        auto acct = accounts(get_self(), user.value);
        auto balance_val = acct.get(sym_code.raw(), "Cannot getbalance for user");
        return balance_val.balance;
    }

    bool pegtoken::balance_check( symbol_code sym_code, name user ) {
        auto acct = accounts(get_self(), user.value);
        auto balance = acct.find(sym_code.raw());
        return balance == acct.end() || balance->balance.amount == 0;
    }
    
    bool pegtoken::addr_check( symbol_code sym_code, name user ) {
        auto addresses = addrs(get_self(), sym_code.raw());
        auto addr_iter = addresses.find(user.value);
        return addr_iter == addresses.end() || (addr_iter->state != 0 || addr_iter->address == "");
    }

    bool pegtoken::getincheck( symbol_code sym_code ) {
        auto check_table = checks(get_self(), sym_code.raw());
        bool incheck = check_table.get(sym_code.raw(), "no such in_check").in_check;
        return incheck;
    }

    bool pegtoken::getoutcheck( symbol_code sym_code ) {
        auto check_table = checks(get_self(),sym_code.raw());
        bool outcheck = check_table.get(sym_code.raw(),"no such out_check").out_check;
        return outcheck;
    }

    bool pegtoken::is_sym_equal_asset( symbol_code sym_code, asset quantity ) {
        symbol_code asset_symcode = quantity.symbol.code();
        return asset_symcode == sym_code;
    }

    bool pegtoken::is_vip( symbol_code sym_code, name name ) {
        auto vip_tb = vips(get_self(), sym_code.raw());
        auto iter = vip_tb.find(name.value);
        return iter != vip_tb.end();
    }

    void pegtoken::is_auth_issuer( symbol_code sym_code ) {
        auto info_table = infos(get_self(), sym_code.raw());
        auto info_val = info_table.get(sym_code.raw(), "the token not in infos table");
        require_auth(info_val.issuer);
    }

    void pegtoken::is_auth_manager( symbol_code sym_code ) {
        auto manager_tb = managers(get_self(), sym_code.raw());
        auto mgr_iter = manager_tb.begin();
        eosio_assert(mgr_iter != manager_tb.end(), "the token not in managers table");
        require_auth(mgr_iter->manager);
    }

    void pegtoken::is_auth_teller( symbol_code sym_code ) {
        auto teller_tb = tellers(get_self(), sym_code.raw());
        auto teller_iter = teller_tb.begin();
        eosio_assert(teller_iter != teller_tb.end(), "the token not in tellers table");
        require_auth(teller_iter->teller);
    }

    void pegtoken::is_auth_auditor( symbol_code sym_code, name auditor ) {
        auto auditor_tb = auditors(get_self(), sym_code.raw());
        auto auditor_iter = auditor_tb.find(auditor.value);
        eosio_assert(auditor_iter != auditor_tb.end(), "auditor not in auditors table.");
        require_auth(auditor_iter->auditor);
    }

    void pegtoken::is_auth_brakeman( symbol_code sym_code ) {
        auto brakeman_tb = brakemans(get_self(), sym_code.raw());
        auto brake_iter = brakeman_tb.begin();
        eosio_assert(brake_iter != brakeman_tb.end(), "the token not in brakemans table");
        require_auth(brake_iter->brakeman);
    }

    void pegtoken::is_auth_gatherer( symbol_code sym_code ) {
        auto gatherer_tb = gatherers(get_self(),sym_code.raw());
        auto gatherer_val = gatherer_tb.get(sym_code.raw(), "the token not in gatherers table");
        require_auth(gatherer_val.gatherer);
    }

    void pegtoken::is_auth_role( symbol_code sym_code, name account ) {
        auto brakeman_tb = brakemans(get_self(), sym_code.raw());
        auto braks = brakeman_tb.find(account.value);
        eosio_assert(braks == brakeman_tb.end(), "account has been assigned to role: brakeman");

        auto auditor_tb = auditors(get_self(), sym_code.raw());
        auto auds = auditor_tb.find(account.value);
        eosio_assert(auds == auditor_tb.end(), "account has been assigned to role: auditor");

        auto manager_tb = managers(get_self(), sym_code.raw());
        auto mgr = manager_tb.find(account.value);
        eosio_assert(mgr == manager_tb.end(), "account has been assigned to role: manager");

        auto teller_tb = tellers(get_self(), sym_code.raw());
        auto tellers = teller_tb.find(account.value);
        eosio_assert(tellers == teller_tb.end(), "account has been assigned to role: teller");        

        auto gatherer_tb = gatherers(get_self(), sym_code.raw());
        auto gat = gatherer_tb.find(account.value);
        eosio_assert(gat == gatherer_tb.end(), "account has been assigned to role: gatherer");    

        auto info_table = infos(get_self(), sym_code.raw());
        auto info_iter = info_table.find(sym_code.raw());
        eosio_assert(info_iter != info_table.end(), "No such symbol");
        eosio_assert(account != info_iter->issuer, "The account has been assigned to issuer");
    }

     void pegtoken::is_auth_role_exc_gatherer( symbol_code sym_code, name account ) {
        auto brakeman_tb = brakemans(get_self(), sym_code.raw());
        auto braks = brakeman_tb.find(account.value);
        eosio_assert(braks == brakeman_tb.end(), "account has been assigned to role: brakeman");

        auto auditor_tb = auditors(get_self(), sym_code.raw());
        auto auds = auditor_tb.find(account.value);
        eosio_assert(auds == auditor_tb.end(), "account has been assigned to role: auditor");

        auto manager_tb = managers(get_self(), sym_code.raw());
        auto mgr = manager_tb.find(account.value);
        eosio_assert(mgr == manager_tb.end(), "account has been assigned to role: manager");

        auto teller_tb = tellers(get_self(), sym_code.raw());
        auto tellers = teller_tb.find(account.value);
        eosio_assert(tellers == teller_tb.end(), "account has been assigned to role: teller");        

        auto info_table = infos(get_self(), sym_code.raw());
        auto info_iter = info_table.find(sym_code.raw());
        eosio_assert(info_iter != info_table.end(), "No such symbol");
        eosio_assert(account != info_iter->issuer, "The account has been assigned to issuer");
    }

    name pegtoken::get_gatherer( symbol_code sym_code ) {
        auto gat_table = gatherers(get_self(), sym_code.raw());
        auto gat_iter = gat_table.begin();
        eosio_assert(gat_iter != gat_table.end(), "No such symbol in gatherers table");
        return gat_iter->gatherer;
    }

    name pegtoken::get_teller( symbol_code sym_code ) {
        auto teller_tb = tellers(get_self(), sym_code.raw());
        auto teller_iter = teller_tb.begin();
        eosio_assert(teller_iter != teller_tb.end(), "the token not in tellers table");
        return teller_iter->teller;
    }
    
    bool pegtoken::is_locked( symbol_code sym_code ) {
        auto info_table = infos(get_self(), sym_code.raw());
        auto info_val = info_table.get(sym_code.raw(), "No such symbol in infos table");
        return info_val.active;
    }

    void pegtoken::vip_withdraw_check( symbol_code sym_code, asset quantity, name account ) {
        auto vlimits_tb = viplimits(get_self(), sym_code.raw());
        auto vlim_val = vlimits_tb.get(account.value, "This type of assets not exists in limits table");

        auto statistics_tb = statistics(get_self(), sym_code.raw());
        auto statistic_iter = statistics_tb.find(account.value);
        auto now_time = time_point_sec(now());
        auto zero_asset = eosio::asset(0, quantity.symbol);
        if(statistic_iter == statistics_tb.end()) {
             statistics_tb.emplace(get_self(), [&](auto &p) {
                p.owner = account;
                p.last_time = now_time;
                p.frequency = 0;
                p.total = zero_asset;
                p.update_time = now_time;
            });
        } else {
             eosio_assert(now_time - statistic_iter->last_time >= seconds(vlim_val.interval_limit), 
                "From now is less than interval_limit");
        }
        auto statistic_val = statistics_tb.get(account.value, "No such account in statistics table");
        time_point_sec lasttime = statistic_val.last_time;
	    uint64_t freq;
	    asset total;
        if(now_time - lasttime > DAY_IN_MICROSECOND) {
            freq = 0;
            total = zero_asset;
        } else {
            freq = statistic_val.frequency;
            total = statistic_val.total;
        }
        eosio_assert(quantity <= vlim_val.maximum_limit, "withdraw amount is more than the maximum_limit");
        eosio_assert(quantity >= vlim_val.minimum_limit, "withdraw amount is less than the minimum_limit");
        eosio_assert(total + quantity <= vlim_val.total_limit, "More than daily totals amount");
        eosio_assert(freq + 1 <= vlim_val.frequency_limit, "More than daily frequency limit");
    }

    void pegtoken::withdraw_check( symbol_code sym_code, asset quantity, name account ) {
        auto limits_tb = limits(get_self(), sym_code.raw());
        auto lim_val = limits_tb.get(sym_code.raw(), "This type of assets not exists in limits table");
       
        auto statistics_tb = statistics(get_self(), sym_code.raw());
        auto statistic_iter = statistics_tb.find(account.value);
        auto now_time = time_point_sec(now());
        auto zero_asset = eosio::asset(0, quantity.symbol);
        if(statistic_iter == statistics_tb.end()) {
            statistics_tb.emplace(get_self(), [&](auto &p) {
                p.owner = account;
                p.last_time = now_time;
                p.frequency = 0;
                p.total = zero_asset;
                p.update_time = now_time;
            });
        } else {
            eosio_assert(now_time - statistic_iter->last_time >= seconds(lim_val.interval_limit),
                "From now is less than interval_limit");
        }
        auto statistic_val = statistics_tb.get(account.value, "No such account in statistics table");
        time_point_sec lasttime = statistic_val.last_time;
        uint64_t freq;
        asset total;
        if(now_time - lasttime > DAY_IN_MICROSECOND) {
		    freq = 0;
       		total = zero_asset;
        } else {
            freq = statistic_val.frequency;
            total = statistic_val.total;
        }
        eosio_assert(quantity <= lim_val.maximum_limit, "withdraw amount is more than the maximum_limit");
        eosio_assert(quantity >= lim_val.minimum_limit, "withdraw amount is less than the minimum_limit");
        eosio_assert(total + quantity <= lim_val.total_limit, "More than daily totals amount");
        eosio_assert(freq + 1 <= lim_val.frequency_limit, "More than daily frequency limit");
    }
} // namespace eosio
