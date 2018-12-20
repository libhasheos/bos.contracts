/**
 *  @file
 *  @copyright defined in bos/LICENSE.txt
 */
#pragma once

#include "decoder.hpp"
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>
#include <string>

#define PRIMARY_KEY(pk) \
    uint64_t primary_key() const { return pk; }

namespace eosio {

using transaction_id_type = capi_checksum256;

struct [[eosio::table]] symbol_ts {
    symbol sym;

    PRIMARY_KEY(sym.code().raw())
};

struct [[eosio::table]] applicant_ts {
    name applicant;

    uint64_t primary_key() const { return applicant.value; }
};

struct [[eosio::table]] rchrgaddr_ts {
    name owner;
    string address;
    uint64_t state;

    time_point_sec assign_time;
    time_point_sec create_time;

    PRIMARY_KEY(owner.value)

    uint64_t by_address() const { return hash64(address); }
    uint64_t by_state() const { return state; }

    rchrgaddr_ts()
        : create_time(now())
    {
    }
};

struct [[eosio::table]] operate_ts {
    uint64_t id;
    name to;
    asset quantity;
    string memo;
    time_point_sec operate_time;

    PRIMARY_KEY(id)

    operate_ts()
        : operate_time(now())
    {
    }
};

struct [[eosio::table]] withdraw_ts {
    uint64_t id;
    transaction_id_type trx_id;
    name from;
    string to;
    asset quantity;
    uint64_t state;

    bool enable;
    name auditor;
    string remote_trx_id;
    string msg;

    time_point_sec create_time;
    time_point_sec update_time;

    PRIMARY_KEY(id)

    fixed_bytes<32> by_trxid() const { return trxid(trx_id); }

    uint128_t by_delindex() const
    {
        uint128_t index = (state == 2 || state == 3) ? 1 : 2;
        index <<= 64;
        index += quantity.amount;
        return index;
    }

    uint128_t by_queindex() const
    {
        uint128_t index = state;
        index <<= 64;
        index += id;
        return index;
    }

    withdraw_ts()
        : create_time(now())
        , update_time(now())
    {
    }

    static fixed_bytes<32> trxid(transaction_id_type trx_id) { return fixed_bytes<32>(trx_id.hash); }
};

struct [[eosio::table]] deposit_ts {
    uint64_t id;
    transaction_id_type trx_id;
    name from;
    string to;
    asset quantity;
    uint64_t state;
    string remote_trx_id;
    string msg;

    time_point_sec create_time;
    time_point_sec update_time;

    PRIMARY_KEY(id)

    uint64_t by_delindex() const { return create_time.utc_seconds; }

    deposit_ts()
        : create_time(now())
        , update_time(now())
    {
    }
};

struct [[eosio::table]] statistic_ts {
    name owner;
    time_point_sec last_time;
    uint64_t frequency;
    asset total;
    time_point_sec update_time;

    PRIMARY_KEY(owner.value);

    statistic_ts()
        : update_time(now())
    {
    }
};

struct [[eosio::table]] account_ts {
    asset balance;

    PRIMARY_KEY(balance.symbol.code().raw())
};

struct [[eosio::table]] stat_ts {
    asset supply;
    asset max_limit;
    asset min_limit;
    asset total_limit;
    uint64_t frequency_limit;
    uint64_t interval_limit;
    uint64_t delayday;
    name issuer;
    name acceptor;
    string organization;
    string website;
    double fee_rate;
    asset min_fee;
    bool active;

    PRIMARY_KEY(supply.symbol.code().raw())
};

struct [[eosio::table]] auditor_ts {
    name auditor;

    PRIMARY_KEY(auditor.value)
};

} // namespace eosio
